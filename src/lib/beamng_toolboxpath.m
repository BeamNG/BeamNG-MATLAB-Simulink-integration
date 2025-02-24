classdef beamng_toolboxpath
    %beamng_toolboxpath  Path to BeamNG toolbox.
    
    properties
        root
    end

    properties (Dependent)
        lib
        examples
    end
    
    methods
        function obj = beamng_toolboxpath()
            p = fileparts(fileparts(mfilename("fullpath")));
            obj.root = p;
        end
        
        function path = get.lib(obj)
            path = fullfile(obj.root,'lib');
        end

        function path = get.examples(obj)
            path = fullfile(obj.root,'examples');
        end
    end
end
