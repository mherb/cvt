function stereoCalibToXML( file )
%STEREOCALIBTOXML Summary of this function goes here
%   Detailed explanation goes here

SC = load(file);
printLeft( SC );
fprintf('\n\n\n');
printRight( SC );

end

function printLeft( SC )


fprintf('<?xml version="1.0" encoding="UTF-8"?>\n');
fprintf('<CameraCalibration>\n');
fprintf('	<Extrinsics>\n');

fprintfMat( eye(4,4));

fprintf('	</Extrinsics>\n');

fprintf('	<Intrinsics>\n');
fprintfMat( SC.KK_left );
fprintf('	</Intrinsics>\n');

fprintf('	<Distortion>\n');
fprintf('		<Radial>\n');

fprintfMat([ SC.kc_left(1:2)' SC.kc_left(5)]');
fprintf('		</Radial>\n');
fprintf('		<Tangential>\n');
fprintfMat(SC.kc_left(3:4));
            
fprintf('		</Tangential>\n');
fprintf('		</Distortion>\n');

fprintf('</CameraCalibration>\n');
end

function printRight( SC )


fprintf('<?xml version="1.0" encoding="UTF-8"?>\n');
fprintf('<CameraCalibration>\n');
fprintf('	<Extrinsics>\n');

fprintfMat( cat(1,[SC.R SC.T],[0 0 0 1]));

fprintf('	</Extrinsics>\n');

fprintf('	<Intrinsics>\n');
fprintfMat( SC.KK_right );
fprintf('	</Intrinsics>\n');

fprintf('	<Distortion>\n');
fprintf('		<Radial>\n');

fprintfMat([ SC.kc_right(1:2)' SC.kc_right(5)]');
fprintf('		</Radial>\n');
fprintf('		<Tangential>\n');
fprintfMat(SC.kc_right(3:4));
            
fprintf('		</Tangential>\n');
fprintf('		</Distortion>\n');

fprintf('</CameraCalibration>\n');
end

function fprintfMat( X )
[n m ] = size(X);
for y=1:n
    for x=1:m
        fprintf('%.8f ',X(y,x));
    end
    fprintf('\n');
end
end

