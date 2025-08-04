function(copy_to_binary_target RootSource Dir FileSelect Target)
    file(GLOB_RECURSE FilesInDir "${RootSource}/${Dir}/${FileSelect}")
    string(LENGTH "${RootSource}/${Dir}/" PathLen)

    foreach (File IN LISTS FilesInDir)
        get_filename_component(FileParentDir ${File} DIRECTORY)
        set(FileParentDir "${FileParentDir}/")

        string(LENGTH "${File}" FilePathLen)
        string(LENGTH "${FileParentDir}" DirPathLen)

        math(EXPR FileTrimmedLen "${FilePathLen}-${PathLen}")
        math(EXPR DirTrimmedLen "${DirPathLen}-${PathLen}")

        string(SUBSTRING ${File} ${PathLen} ${FileTrimmedLen} FileStripped)
        string(SUBSTRING ${FileParentDir} ${PathLen} ${DirTrimmedLen} DirStripped)

        set(BINARY_DIR "${PROJECT_BINARY_DIR}")
        if (APPLE)
            set(BINARY_DIR "${PROJECT_BINARY_DIR}/${Target}.app/Contents/Resources")
        endif ()
        file(MAKE_DIRECTORY "${BINARY_DIR}/${Dir}")
        file(MAKE_DIRECTORY "${BINARY_DIR}/${Dir}/${DirStripped}")
        configure_file(${File} ${BINARY_DIR}/${Dir}/${DirStripped} COPYONLY)
    endforeach ()
endfunction()

function(copy_to_binary RootSource Dir FileSelect)
    copy_to_binary_target(${RootSource} ${Dir} ${FileSelect} ${PROJECT_NAME})
endfunction()