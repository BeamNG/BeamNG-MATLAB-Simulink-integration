% Use this file to package the BeamNG toolbox
% this is needed to correclty add all the subfolders to the MATLAB path in
% order to have the Simulink blocks in the Library Browser

close all
clear
clc

addpath(genpath(pwd))

%% Publish examples
examples_folder = 'src/examples/matlab/';
examples_files = {'annotation_bounding_boxes'
                  'lidar_tour'
                  'multishot_camera'
                  'object_placement'
                  'vehicle_state_plotting'};

for i = 1:length(examples_files)
    [~] = export(strcat(examples_folder, examples_files{i}, '.mlx'), ...
        strcat(examples_folder, 'html/', examples_files{i},'.html'));
end

examples_folder = 'src/examples/simulink/';
examples_files = {'simulink_demos'};

for i = 1:length(examples_files)
    [~] = export(strcat(examples_folder, examples_files{i}, '.mlx'), ...
        strcat(examples_folder, 'html/', examples_files{i},'.html'));
end

%% Publish documentation
doc_folder = 'doc/';
src_doc_folder = 'src/doc/html/';
[~] = export(strcat(doc_folder, 'GettingStarted.mlx'), strcat(src_doc_folder, 'GettingStarted.html'));
documentation_files = {'documentation_home'
                       's_function'
                       'setting_up_simulink'
                       'simulink_interface'};

for i = 1:length(documentation_files)
    [~] = publish(strcat(doc_folder, documentation_files{i}), 'outputDir', src_doc_folder);
end

%% Package toolbox
projectFile = 'MATLAB and Simulink Support for BeamNG.tech.prj';
outputFile = 'MATLAB and Simulink Support for BeamNG.tech.mltbx';

matlab.addons.toolbox.packageToolbox(projectFile, outputFile)

disp(strcat("Toolbox correctly packaged and saved as: ", outputFile))

rmpath(genpath(pwd))