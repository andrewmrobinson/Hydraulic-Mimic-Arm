clear all
close all
clc
data = [];
pressures = [0.6 0.8 1.0 1.2 1.4 1.6 1.8]*10^6;
%pressures = [1.8]*10^6;
for ll = 1:length(pressures)
    %P = 1.8*10^6; %18 bar
    P = pressures(ll);
    Acyl = (0.110 - 0.028)*(2.54^2)/10000; %Bore Area - Rod Area in m^2
    fric_coeff = 0.492;
    assumed_angle = 45; %degrees
    finger_names = {'Index' 'Middle' 'Ring' 'Pinky'};
    r_lower = [7.7700 9.6750 8.3850 8.3850]/1000;
    r_upper = [8.2500 8.2050 8.2350 7.8600]/1000;
    R_lower = [50.8950 53.9250 52.5600 50.1750]/1000;
    R_upper = [18.1050 16.8900 18.0300 21.0150]/1000;
    guide_angles = [16.6 8.3 3 0 6.3 16.7 18 26.7];
    hand_angles = [30 19.6 7 0 0 12.4 23 34];
    
    for ii = 1:length(finger_names)
        %F_Upper
        Fcyl = P*Acyl;
        Fcyl = 38.868*(P/1000000) + 0.6985;

        
        N1 = Fcyl*cosd((180-guide_angles(2*ii-1))/2); % wire guide at end of potentiometers
        N2 = Fcyl*cosd((180-hand_angles(2*ii-1))/2); % into hand
        Loss_friction = N1*fric_coeff + N2*fric_coeff;
        F_upper(ii) = Fcyl - Loss_friction;
        
        %F_Lower
        
        Fcyl = P*Acyl;
        Fcyl = 38.868*(P/1000000) + 0.6985;
        N1 = Fcyl*cosd((180-guide_angles(2*ii))/2); % wire guide at end of potentiometers
        N2 = ((Fcyl-N1*fric_coeff)*2)*cosd((180-hand_angles(2*ii))/2); % into hand
        
        F_lower(ii) = ((Fcyl-N1*fric_coeff)*2) - N2*fric_coeff;
        
        F_total(ii) = ((F_lower(ii) * r_lower(ii))/R_lower(ii)) + ((F_upper(ii) * r_upper(ii))/R_upper(ii));
        %fprintf('%s finger: %fN\n',finger_names{ii},F_total(ii));
    end
    fprintf('%dPa - %fN\n',P,sum(F_total))
    data(end+1,1:2) = [P*10^-6 sum(F_total)/9.81];
end
hold on
plot(data(:,1),data(:,2))
plot(data(:,1),data(:,2),'r+')
title('Theoretical Horizontal Force from Middle Phalanx');
xlabel('Pressure (MPa)');
ylabel('Equivalent Dynamometer Force (kgs)');
axis([0.4 2 0 max(data(:,2))+2]);
