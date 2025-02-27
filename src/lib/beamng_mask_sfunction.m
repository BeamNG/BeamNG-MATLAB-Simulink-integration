classdef beamng_mask_sfunction
    % Mask callbacks for BeamNG S-funcion block.
    % If you view this file in the Simulink mask editor it is a view of
    % file beamng_mask_sfunction.m
    % This file is used in the Mask definition of the S-function block in
    % BeamNG_interface.slx.

    methods(Static)
        function MaskInitialization(maskInitContext)
            maskWorkspace = maskInitContext.MaskWorkspace;

            % Convert user provided CSV file path to absolute path
            csvFilePath = maskWorkspace.get('csvFilePath');
            csvFullPath = getFullFilePath(csvFilePath);
            maskWorkspace.set('csvFullFilePath', csvFullPath);
        end
    end
end

function fullpath = getFullFilePath(filepath)
% Convert relative file path to absolute file path.
% - if filepath is an empty string -> use default interface file
% - if filepath is already an absolute path -> use it
% - otherwise: convert relative path to absolute path with
%   respect to current working directory
if isempty(filepath)
    assets_path = beamng_toolboxpath().assets;
    fullpath = fullfile(assets_path, 'default_interface.csv');
elseif is_absolute_path(filepath)
    fullpath = filepath;
else
    fullpath = canonical_path(fullfile(pwd, filepath));
end
end

function b = is_absolute_path(filepath)
b = java.io.File(filepath).isAbsolute();
end

function p = canonical_path(fullpath)
% get rid of extra . or .. path components
p = char(java.io.File(fullpath).getCanonicalPath());
end
