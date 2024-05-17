classdef beamng_interface_callbacks
    methods(Static)
        function csvFilePath(callbackContext)
            parameterObj = callbackContext.ParameterObject;
            filename = convertCharsToStrings(parameterObj.Value);
            filenameParts = split(filename, "'");
            if length(filenameParts) ~= 3
                return
            end
            
            filename = filenameParts{2};
            
            % current directory
            [status, info] = fileattrib(filename);
            if status
                filename = "'" + info.Name + "'";
                parameterObj.Value = convertStringsToChars(filename);
                return
            end

            % library directory
            global BeamNGCosimDir
            if ~BeamNGCosimDir
                BeamNGCosimFilename = get_param(bdroot('BeamNG_interface/BeamNG_CoSimulation'), 'FileName');
                [BeamNGCosimDir, ~, ~] = fileparts(BeamNGCosimFilename);
            end

            [status, info] = fileattrib(fullfile(BeamNGCosimDir, filename));
            if status
                filename = "'" + info.Name + "'";
                parameterObj.Value = convertStringsToChars(filename);
                return
            end
        end
    end
end