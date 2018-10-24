%2018 FYP - Hydraulic Hand
%Andrew Robinson

function rgb = return_rgb(min,max,value)
%Returns an RGB value which scales through the rainbow dependent on where
%the value lies between a min and a max
%Inputs:
% min - scalar number
% max - scalar number
% value - scalar number
%
%Ouputs:
% rgb - vector [r g b]

if(min>=max)
    error('Min must be smaller than max')
elseif value<min || value>max
    error('value must be between min and max')
end

normalised_val = ((value-min)/(max-min) *2)-1;
%red
if normalised_val>0 && normalised_val< 0.5
    red = 2*normalised_val;
elseif normalised_val>=0.5
    red = 1;
else
    red = 0;
end

%green
if normalised_val>-0.5 && normalised_val< 0.5
    green = 1;
elseif normalised_val>=0.5
    green = -2*normalised_val+2;
else
    green = 2*normalised_val+2;
end
%blue
if normalised_val>-0.5 && normalised_val< 0
    blue = -2*normalised_val;
elseif normalised_val>=0
    blue = 0;
else
    blue = 1;
end



rgb = [red green blue];
end