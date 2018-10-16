clear all
close all
clc
P = 1.8*10^6; %18 bar
Acyl = (0.110 - 0.028)*(2.54^2)/10000; %Bore Area - Rod Area in m^2
fric_coeff = 0.492;
assumed_angle = 45; %degrees

F_total = ((F_lower * r_lower)/R_lower) + ((F_upper * r_upper)/R_upper);
r_lower = [
r_upper
R_lower
R_upper
guide_angles = [16.6 8.3 3 0 6.3 16.7 18 26.7];
hand_angles = [30 19.6 7 0 0 12.4 23 34];
%F_Upper

Fcyl = P*Acyl;

N1 = Fcyl*cosd((180-wire_angle)/2); % wire guide at end of potentiometers
N2 = Fcyl*cosd((180-wire_angle)/2); % into hand
Loss_friction = N1*fric_coeff + N2*fric_coeff;
F_upper = Fcyl - Loss_friction;

%F_Lower

Fcyl = P*Acyl;

N1 = Fcyl*cosd((180-wire_angle)/2); % wire guide at end of potentiometers
N2 = ((Fcyl-N1*fric_coeff)*2)*cosd((180-wire_angle)/2); % into hand
Loss_friction = N1*fric_coeff + N2*fric_coeff;

F_lower = ((Fcyl-N1*fric_coeff)*2) - N2*fric_coeff;
