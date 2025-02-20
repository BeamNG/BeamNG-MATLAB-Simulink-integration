function cleanup()
%CLEANUP   Delete files ignored by git.
%This should be called before generating the toolbox to not include any
%temporary "slprj" folders or other automatically generated files.
status = system('git clean -dfX');
if status ~= 0
    error('Error during cleanup. Could not clean up files.')
end
end
