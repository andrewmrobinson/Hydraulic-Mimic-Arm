clear all
close all
clc

width=300;
height=200;
x0=10;
y0=10;

pressures = [0.6 0.8 1 1.2 1.4 1.6 1.8]; %MPa
dyna_forces = [4 5.5 5.9 7 8 9.75 10.5]; %kg
theoretical = [8.885037717 11.84671764 14.80839653 17.77007645 20.73175535 23.69343527 26.65511417];
semi_theoretical = [6.72337105 8.899321101 11.07527217 13.25122222 15.42717227 17.60312232 19.77907238];

figure;plot(pressures,dyna_forces,...
    pressures,theoretical,...
    pressures,semi_theoretical);
xlabel('Hydraulic System Pressure (MPa)');
ylabel('Power Grasp Force (kg)');
%title('Dynamometer Power Grasp force');
legend('Dynamometer','Model','Adjusted Model','location','northwest');
xlim([0.4 2]);


set(gcf,'units','points','position',[x0,y0,width,height])
print(gcf,[ 'dynamometer_grasp_force.png'],'-dpng');

pressures = [0.4 pressures];
exp_force = [16.43 24 31 41 47.5 54 61.7 72];
theoretical_force = [21.161248 31.741872 42.322496 52.90312 63.483744 74.064368 84.644992 95.225616];
figure;plot(pressures,exp_force,...
    pressures,theoretical_force);
xlabel('Hydraulic System Pressure (MPa)');
ylabel('Force (N)')
%title('Hydraulic Cylinder Retraction Force');

yyaxis right
plot(pressures,exp_force./theoretical_force,'r--');
legend('Force Gauge','Theoretical','Efficiency','location','southeast');
ylim([0 1]);
xlim([0.2 2]);
set(gcf,'units','points','position',[x0,y0,width,height])
print(gcf,[ 'cylinder_retraction_force.png'],'-dpng');

figure
force = [44.876053 58.208644 51.223193 39.724816];
names={'Index'; 'Middle'; 'Ring' ;'Little'};
bar(force)
ylabel('Force (N)');
set(gca,'xticklabel',names)
set(gcf,'units','points','position',[x0,y0,width,height])
print(gcf,[ 'finger_forces.png'],'-dpng');
close all
            