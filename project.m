function opts = project()
identifier = 'bc63248b-ec48-40d5-8ace-08d22c71f179';
source_folder = 'src';
opts = matlab.addons.toolbox.ToolboxOptions(source_folder,identifier);
opts.ToolboxName = 'BeamNG.tech Support for MATLAB and Simulink';
opts.ToolboxVersion = '1.4.0';
opts.AuthorEmail = 'tech@beamng.gmbh';
opts.AuthorName = 'BeamNG GmbH';
opts.AuthorCompany = 'BeamNG GmbH';
opts.Summary = 'Connects MATLAB and Simulink to BeamNG.tech';
opts.Description = 'This toolbox allows you to connect MathWorks products with BeamNG.tech.';
opts.ToolboxImageFile = 'src/assets/BeamNG-complete-logo.png';
opts.ToolboxGettingStartedGuide = 'src/assets/GettingStarted.mlx';
opts.ToolboxMatlabPath = {'src/lib'};
opts.MinimumMatlabRelease = 'R2023b';
opts.MaximumMatlabRelease = '';
opts.SupportedPlatforms.Win64 = true;
opts.SupportedPlatforms.Glnxa64 = true;
opts.SupportedPlatforms.Maci64 = false;
opts.SupportedPlatforms.MatlabOnline = false;
opts.OutputFile = 'release/BeamNG.tech-Support-for-MATLAB-and-Simulink.mltbx';
end
