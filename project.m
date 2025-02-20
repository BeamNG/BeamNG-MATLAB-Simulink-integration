function opts = project()
identifier = 'bc63248b-ec48-40d5-8ace-08d22c71f179';
source_folder = 'src';
opts = matlab.addons.toolbox.ToolboxOptions(source_folder,identifier);
opts.ToolboxName = 'BeamNG.tech Support for MATLAB and Simulink';
opts.ToolboxVersion = '1.3.1.1';
opts.AuthorEmail = 'tech@beamng.gmbh';
opts.AuthorName = 'BeamNG GmbH';
opts.AuthorCompany = 'BeamNG GmbH';
opts.Summary = 'Connects MATLAB and Simulink to BeamNG.tech';
opts.Description = 'MATLAB and Simulink Support for BeamNG.tech allows you to connect the Mathworks products with BeamNG.tech.';
opts.ToolboxImageFile = 'src/pictures/BeamNG-complete-logo.png';
opts.ToolboxGettingStartedGuide = 'src/doc/GettingStarted.mlx';
opts.MinimumMatlabRelease = 'R2023b';
opts.MaximumMatlabRelease = '';
opts.SupportedPlatforms.Win64 = true;
opts.SupportedPlatforms.Glnxa64 = true;
opts.SupportedPlatforms.Maci64 = false;
opts.SupportedPlatforms.MatlabOnline = false;
opts.OutputFile = 'release/BeamNG.tech Support for MATLAB and Simulink.mltbx';
end
