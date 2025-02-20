function make_release()
%MAKE_RELEASE   Package the toolbox in the release folder.
release_dir = 'release';

% Cleanup
cleanup()
if isfolder(release_dir)
    rmdir(release_dir,'s')
end
mkdir(release_dir)

% Package toolbox
matlab.addons.toolbox.packageToolbox(project())
end
