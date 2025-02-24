function setup_beamngpy(executable_path)
    % setup_beamngpy is a function that takes as input a path to a
    % python.exe file of an environment with BeamNGpy installed. As a first
    % step this function tries to connect to the python environment and
    % prints the detected python version, then tries to import the BeamNGpy
    % module. If no path is given as argument, it will use the current
    % default Python environment.
    %
    % If both the steps are successful the output in the command window
    % should be something like this:
    %
    % --------- Start BeamNGpy setup ---------
    %  
    % Python environment connected
    % Detected version: 3.9
    %  
    % BeamNGpy detected
    %  
    % --------- BeamNGpy setup done ----------
    
    disp(" ")
    disp("--------- Start BeamNGpy setup ---------")
    disp(" ")

    if nargin < 1
        pe = pyenv();
    else
        pe = pyenv(Version=executable_path);
    end

    disp("Python environment connected")
    disp(strcat("Detected version: ", pe.Version))
    disp(" ")

    py.importlib.import_module('beamngpy');

    disp("BeamNGpy detected")
    disp(" ")
    disp("--------- BeamNGpy setup done ----------")
end