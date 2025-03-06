classdef toolboxpath
    %beamng.toolboxpath  Path to BeamNG toolbox.
    
    properties
        root
    end

    properties (Dependent)
        lib  % this is the only folder added to MATLAB path
        examples
        assets
    end
    
    methods
        function obj = toolboxpath()
            p = fileparts(fileparts(fileparts(mfilename("fullpath"))));
            obj.root = p;
        end
        
        function path = get.lib(obj)
            path = fullfile(obj.root,'lib');
        end

        function path = get.examples(obj)
            path = fullfile(obj.root,'examples');
        end

        function path = get.assets(obj)
            path = fullfile(obj.root,'assets');
        end
    end
end
