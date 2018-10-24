clear all
close all
clc
%2018 FYP - Hydraulic Hand
%Andrew Robinson

subplot = @(m,n,p) subtightplot (m, n, p, [0.05 0.05], [0.1 0.05], [0.1 0.01]);
%Reduces subplot margins

%Chosen plots
specifics = [1,1;...
    2,1;...
    3,1;...
    8,1;...
    8,4;...
    7,1;...
    9,1];

%Time limits for x values
time_limits = [9,16.5;...
    7,14;...
    10,18;...
    7,14;...
    1,9.5;...
    12,25;...
    6.3,14];

%Y limits
error_max = [700 700 200 700 700 200 800];
titles = {'Single Finger Step Response PID={0.4, 0.01, 0.02}'...
    'Single Finger Step Response PID={0.4, 0.1, 0.2}'...
    'Single Finger Ramp Response PID={0.4, 0.1, 0.02}'...
    'Leap Motion Sensor Follow (Cyl 0)'...
    'Leap Motion Sensor Follow (Cyl 3)'...
    'Ramp Response (all cylinders running)'...
    'Leap Motion Sensor Follow (Cyl 0)'};
    
    
filenames = {'single_finger0_p0.4i0.01d0.02_step.csv'...
    'single_finger0_p0.4i0.1d0.2_step_loaded.csv'...
    'single_finger0_p0.4i0.1d0.2_ramp.csv'...
    'step COM5_2018-10-08 170610.csv'...
    'step COM3_2018-10-08 170610.csv'...
    'ramp COM5_2018-10-08 170046.csv'...
    'ramp COM3_2018-10-08 170046.csv'...
    'hand follow COM5_2018-10-08 170356.csv'...
    'hand follow COM3_2018-10-08 170356.csv'};

for j = 1:length(filenames)
    [data, ~, raw_data] = xlsread(filenames{j});
    %data = data(600:end,:);
    time_step = 8 /1000; %8 ms
    t = [1:size(data,1)]'*time_step;
    
    no_cyl = 4;
    for i = 1:no_cyl
        for k = 1:size(specifics,1)
            if (max(j==specifics(k,1)) && max(i==specifics(k,2)))
                data = data(t>=time_limits(k,1)&t<=time_limits(k,2),:);
                t = t(t>=time_limits(k,1)&t<=time_limits(k,2),:);
                %error(:,i) = abs(data(:,i+4)-data(:,i)); %desired-actual
                error = [];
                error = abs(data(:,i+4)-data(:,i)); %desired-actual
                error(isnan(error)) = 0;
                %avg_error = ones(size(data,1),1).*mean(error(:,i));
                avg_error = ones(size(data,1),1).*mean(error);
                
                figure
                x0=10;
                y0=10;
                width=500;
                height=400;
                set(gcf,'units','points','position',[x0,y0,width,height])
                %plot(t,error(:,i),t,avg_error,'r');
                subplot(2,1,1);
                plot(t,error,'b',t,avg_error,'r--');
                title(titles{k});
                %title([filenames{j} ' - Error of Cyl ' num2str(i-1)]);
                axis([min(t) max(t) 0 error_max(k)]);
                legend('Error','Mean Error','Location','NorthEast');
                ylabel('Error');
                subplot(2,1,2);
                plot(t,data(:,i),'b',t,data(:,i+4),'r-.');
                xlim([min(t) max(t)]);
                legend('Position', 'Goal Position','Location','NorthEast');
                xlabel('Time (s)');
                ylabel('Potentiometer Value (0-4095)');
                
                print(gcf,[num2str(k) ' - refined - ' filenames{j} ' - Error of Cyl ' num2str(i-1) '.png'],'-dpng');
            end
        end
    end
end
close all
