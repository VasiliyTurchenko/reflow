    # PC-Lint target
    if(COMMAND add_pc_lint)
        # establish list of compile defifntiond
        set(comp_defs_list)
        list(APPEND comp_defs_list ${D_MCU})
        list(APPEND comp_defs_list ${D_MCU_EXACT})
        list(APPEND comp_defs_list ${D_HSE_VAL})

        # fantastic loop
        foreach(s IN LISTS ${EXEC_NAME}_LIST_OF_SOURCES)

            set(t_name)
            string(REPLACE "/" "_" t_name ${s})

            add_pc_lint(
                # compile defines
                "${comp_defs_list}"
                # target
                ${t_name} ${EXEC_NAME} ${s})
        endforeach()
    endif(COMMAND add_pc_lint)
