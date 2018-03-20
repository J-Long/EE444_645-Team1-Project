clc;clear; clf;close all;
% type "tmtool" into the command window and delete open serial object
% Change thee 'COM8' below to whatever COM port the MSP is on.
obj = serial('COM8','BaudRate',115200);

time = linspace(0,0.0143*999,1000);
figurel
hold all;
ax = plot(time,data);
ay = plot(time,data);
az = plot(time,data);
eda = plot(time,data);
ppg = plot(time,data);

xlabel('time(s)')
ylabel('ADC value')
axis([0 14 0 4500])
i=1;
fopen(obj)
pause(0.1)
while(collectData == 1)
    if(obj.BytesAvailable ~= 0)
        parse = strsplit(fscanf(obj));            % delimit the string at whitespace
        if parse(1) = ACCX
            ax.YData = str2num(parse(2));
        elseif parse(1) = 'ACCY'
            ay.YData = str2num(parse(2));
        elseif parse(1) = 'ACCZ'
            az.YData = str2num(parse(2));
        elseif parse(1) = 'EDA'
            eda.YData = str2num(parse(2));
        elseif parse(1) = 'PPG'
            ppg.YData = str2num(parse(2));
        else

        end

        drawnow
    end
end
