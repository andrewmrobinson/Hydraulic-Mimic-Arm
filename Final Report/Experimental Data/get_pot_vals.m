close all
clear all
clc
%2018 FYP - Hydraulic Hand
%Andrew Robinson

%The code plots the finger mapping algorithm used in the project, converted
%from the C++ code below.
%
% positions_0[0] = -753.25 * angles.index[0] + 2051.8; // index
% positions_0[1] = -754.84 * angles.middle[0] + 2252.7; // middle
% positions_0[2] = -742.33 * angles.ring[0] + 2064.5; // ring
% positions_0[3] = -843.35 * angles.pinky[0] + 2220.7; // pinky
%
% positions_1[0] = (angles.index[0] / 1.232)	*(-860.17 * angles.index[1] + 2137.9) + (1 - (angles.index[0] / 1.232)) * (-763.63 * angles.index[1] + 2768.9);
% positions_1[1] = (angles.middle[0] / 1.40)	*(-753.43 * angles.middle[1] + 2173.6) + (1 - (angles.middle[0] / 1.40))  * (-750.32 * angles.middle[1] + 2845.0);
% positions_1[2] = (angles.ring[0] / 1.377)*(-703.44 * angles.ring[1] + 2065.4) + (1 - (angles.ring[0] / 1.377)) * (-752.06 * angles.ring[1] + 2707.1);
% positions_1[3] = (angles.pinky[0] / 1.29) *(-705.92 * angles.pinky[1] + 2182.4) + (1 - (angles.pinky[0] / 1.29))  * (-790.62 * angles.pinky[1] + 2852.7);

finger_names = {'Index' 'Middle' 'Ring' 'Pinky'};
%[MCP PIP] joint
max_angles = [1.239183769 1.417207353;...
    1.499237827 1.523672437;...
    1.249655744 1.425933999;...
    1.291543646 1.438151304];
%(prox_angle / max_angles(k,1))	*(A * mid_angle + B) + (1 - (prox_angle / max_angles(k,1))) * (C * mid_angle + D);
% [ A B C D]
formula_coeff = [-860.17 2137.9 -763.63 2768.9;...
    -753.43 2173.6 -750.32 2845.0;...
    -703.44 2065.4 -752.06 2707.1;...
    -705.92 2182.4 -790.62 2852.7];
lower_coeff = [-753.25 2051.8;...
    -754.84 2252.7;...
    -742.33 2064.5;...
    -843.35 2220.7];
for k = 1:4
    prox_angle = linspace(0,max_angles(k,1),50);
    mid_angle = linspace(0,max_angles(k,2),50);
    
    plot_3dvals = [];
    lower_plot_vals = [];
    Z = [];
    X = [];
    Y = [];
    for x = 1:length(prox_angle)
        for y = 1:length(mid_angle)
            pot_val = (prox_angle(x) / max_angles(k,1))	*(formula_coeff(k,1) * mid_angle(y) + formula_coeff(k,2)) + (1 - (prox_angle(x) / max_angles(k,1))) * (formula_coeff(k,3) * mid_angle(y) + formula_coeff(k,4));
            plot_3dvals(end+1,1:3) = [rad2deg(prox_angle(x)) rad2deg(mid_angle(y)) pot_val];
            X(x) = rad2deg(prox_angle(x));
            Y(y) = rad2deg(mid_angle(y));
            Z(y,x) = pot_val;
        end
        pot_val= (lower_coeff(k,1) * prox_angle(x) + formula_coeff(k,2));
        lower_plot_vals(end+1,1:2) =  [rad2deg(prox_angle(x)) pot_val];
    end
    max_pot = max(plot_3dvals(:,3));
    min_pot = min(plot_3dvals(:,3));
    colours = [];
    for i = 1:size(plot_3dvals,1)
        colours(end+1,1:3) = return_rgb(min_pot,max_pot,plot_3dvals(i,3));
    end
    figure
    %scatter3(plot_3dvals(:,1),plot_3dvals(:,2),plot_3dvals(:,3),5,colours,'+');
    surf(X,Y,Z);
    title([finger_names{k} ' Finger - Middle and Distal Phalanx Mapping']);
    xlabel(['Metacarpal-Proximal Angle (' char(176) ')']);
    ylabel(['Proximal-Middle Angle (' char(176) ')']);
    zlabel('Mapped Cylinder Rod position');
    axis([0 100 0 100 800 3000]);
    view(109.6000,30);
    print(gcf,[finger_names{k} ' Finger - Middle and Distal Phalanx Mapping' '.png'],'-dpng');
    
    figure
    plot(lower_plot_vals(:,1),lower_plot_vals(:,2));
    title([finger_names{k} ' Finger - Proximal Phalanx Mapping']);
    xlabel(['Metacarpal-Proximal Angle (' char(176) ')']);
    ylabel('Mapped Cylinder Rod position');
    print(gcf,[finger_names{k} ' Finger - Proximal Phalanx Mapping' '.png'],'-dpng');
    close all
end