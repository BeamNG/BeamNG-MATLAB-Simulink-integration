function copy_examples()
%BEAMNG.COPY_EXAMPLES  Copy BeamNG examples to current working directory.
target_dir = 'examples';
if isfile(target_dir) || isfolder(target_dir)
    msg = strjoin({
        'Cannot copy examples, because the current directory already has '
        'a folder named "examples". Change to a different directory or '
        'rename the existing folder and try again.'
    },'');
    error(msg)
end
source_dir = beamng.toolboxpath().examples;
copyfile(source_dir,target_dir);
disp('Copied BeamNG examples to "examples" folder in the current working directory.');
end
