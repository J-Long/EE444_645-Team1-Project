clc;clear; clf;close all;
% type "tmtool" into the command window and delete open serial object
% Change thee 'COM8' below to whatever COM port the MSP is on.
obj = serial('COM8','BaudRate',115200);

data = zeros(1000,1);
time = linspace(0,0.0143*999,1000);
h = plot(time,data);
xlabel('time(s)')
ylabel('ADC value')
axis([0 14 0 4500])
i=1;
fopen(obj)
pause(0.1)
while(obj.BytesAvailable ~= 0)
    data(i) = str2num(fscanf(obj));
        i = i+1;
    if i == 1000
        i = 1;
    end
    h.YData = data;
    drawnow
end
