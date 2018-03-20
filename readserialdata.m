clc;clear; clf;close all;
% type "tmtool" into the command window and delete open serial object
% Change thee 'COM8' below to whatever COM port the MSP is on.
obj = serial('COM14','BaudRate',115200);

% initialize the data vectors
axdata = zeros(1000,1);
aydata = zeros(1000,1);
azdata = zeros(1000,1);
edadata = zeros(1000,1);
ppgdata = zeros(1000,1);

% setup the time vector
time = linspace(0,0.0143*999,1000);

% setup the figures
subplot(3,1,1)
hold all;
ax = plot(time,axdata);
ay = plot(time,aydata);
az = plot(time,azdata);
axis([0 14 0 4500])
ylabel('ACC')
subplot(3,1,2)
eda = plot(time,edadata);
axis([0 14 0 4500])
ylabel('EDA')
subplot(3,1,3)
axis([0 14 0 4500])
ppg = plot(time,ppgdata);
axis([0 14 0 4500])
ylabel('PPG')

fopen(obj)
pause(0.1)
while(obj.BytesAvailable ~= 0)
    parse = strsplit(fscanf(obj));            % delimit the string at whitespace
    if cell2mat(strfind(parse(1),'ACCX')) ~= 0
        axdata = circshift(axdata,1);
        axdata(1) = str2num(cell2mat(parse(2)));
        ax.YData = axdata;
    elseif cell2mat(strfind(parse(1),'ACCY')) ~= 0
        aydata = circshift(aydata,1);
        aydata(1) = str2num(cell2mat(parse(2)));
        ay.YData = aydata;
    elseif cell2mat(strfind(parse(1),'ACCZ')) ~= 0
        azdata = circshift(azdata,1);
        azdata(1) = str2num(cell2mat(parse(2)));
        az.YData = azdata;
    elseif cell2mat(strfind(parse(1),'EDA')) ~= 0
        edadata = circshift(edadata,1);
        edadata(1) = str2num(cell2mat(parse(2)));
        eda.YData = edadata;
    elseif cell2mat(strfind(parse(1),'PPG')) ~= 0
        ppgdata = circshift(ppgdata,1);
        ppgdata(1) = str2num(cell2mat(parse(2)));
        ppg.YData = ppgdata;
    else
        % do nothing
    end
    drawnow
end
