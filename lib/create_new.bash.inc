##======================================================
##~For creating a new shcf-based shell project.
##~Usage: shcf_cli new <project_name>
##~ * project_name - cannot have an 'shcf' in it's name.
##~                  Depending on usage, can be anywhere
##~                  you have permission to write.
##~
##======================================================

create_new() {
    ## load dependencies
    autoload_functions "usage create_bin case_shift error_message create_namespace"

    ## function parameters
    local project=$(case_shift low $1)

    ## show help if no parameters given
    [[ $# -eq 0 ]] && { echo "ERROR: Parameter(s) missing!"; usage new; exit $FAILED; }

    ##
    ## core logic below...
    ##
    ## Ban usage of 'shcf' in project name. Reserve for communicating with shcf tool
    [[ $(echo $project | $GREP_BIN -c shcf) -gt 0 ]] && {
      error_message "ERROR: Reserved name 'shcf' cannot be used. Aborting.";
    }

    ## put projects in predictable location by default
    ## TODO: Allow from outside using a switch
    local project_root=$SHCF_PLATFORM_ROOT/projects/$project

    ## Ensure no conflicting namespace in $rc_file
    local prj_namespace=$(create_namespace $project)
    local rc_entry="${prj_namespace}=$project_root"
    if [[ ! -e $rc_file ]]; then
       echo $rc_entry > $rc_file
    else
       local prj_exist=$(grep -c ${prj_namespace} $rc_file)
       [[ $prj_exist -gt 0 ]] && { error_message "ERROR: Project $project exist. Aborting."; } || echo $rc_entry >> $rc_file
    fi

    ## copy template for new shcf
    local template_rootdir=$SHCF_PLATFORM_ROOT/etc/tpl/shcf_cli/new

    mkdir -p $project_root
    cp -Rfp $template_rootdir/* $project_root
    local initial_skeleton=$?

    new_entity=$($BASENAME_BIN $project)
    create_bin $project $new_entity
    if [[ $initial_skeleton -eq $SUCCESS ]] && [[ $? -eq $SUCCESS ]]; then
       echo "Successful creation of $project"
       . $rc_file      ## to refresh what is known to shcf without consulting $rc_file
    else
       error_message "Failure creating $project"
    fi
} ## END: create_new()