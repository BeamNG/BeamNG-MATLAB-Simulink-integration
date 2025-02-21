% Use this file to package the BeamNG toolbox
% this is needed to correclty add all the subfolders to the MATLAB path in
% order to have the Simulink blocks in the Library Browser

close all
clear
clc
cleanup

addpath(genpath(pwd))

%% Publish documentation
doc_folder = 'doc/';
src_doc_folder = 'src/doc/html/';
%[~] = export(strcat(doc_folder, 'GettingStarted.mlx'), strcat(src_doc_folder, 'GettingStarted.html'));
documentation_files = {'documentation_home'
                       's_function'
                       'setting_up_simulink'
                       'simulink_interface'};

for i = 1:length(documentation_files)
    [~] = publish(strcat(doc_folder, documentation_files{i}), 'outputDir', src_doc_folder);
end

%% Package toolbox
projectFile = 'BeamNG.tech Support for MATLAB and Simulink.prj';
outputFile = 'BeamNG.tech Support for MATLAB and Simulink.mltbx';

matlab.addons.toolbox.packageToolbox(projectFile, outputFile)

disp(strcat("Toolbox correctly packaged and saved as: ", outputFile))

rmpath(genpath(pwd))