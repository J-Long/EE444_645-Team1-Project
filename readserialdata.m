clc;clear; clf;close all;
% type "tmtool" into the command window and delete open serial object
% Change thee 'COM8' below to whatever COM port the MSP is on.

port = 'COM16';                         % Port that device is on
delete(instrfind('Port',port))          % delete any object using COM14
obj = serial(port,'BaudRate',115200);   % Check if the port is being used still
dt = 0.02;

% setup the time vector
time = 0:seconds(dt):minutes(30); 

% Set the ADC conversion factors
Vm = 0;                                 % Negative reference voltage
Vp = 3;                               % Postiive reference voltage

% setup filter parameters for eda data
windowSize = 100; 
b = (1/windowSize)*ones(1,windowSize);
a = 1;

% initialize the data vectors
axdata = zeros(length(time),1);
aydata = zeros(length(time),1);
azdata = zeros(length(time),1);
edadata = ones(length(time),1).*3;
ppgdata = zeros(length(time),1);

% setup the figures
subplot(3,1,1)
hold all;
ax = plot(time,axdata);
ay = plot(time,aydata);
az = plot(time,azdata);
legend('X','Y','Z')
ylim([-3 3]);
ylabel('Acceleration (g)')
xtickformat('mm:ss')
subplot(3,1,2)
eda = plot(time,edadata);
ylabel('EDA (V)')
ylim([0 3]);
xtickformat('mm:ss')
subplot(3,1,3)
ppg = plot(time,ppgdata);
ylabel('PPG (mV)') 
ylim([0 3]);
xtickformat('mm:ss')
hold off;

if(strcmp(obj.status,'open')==0)
    fopen(obj)
else
    fclose(obj)
    disp('Object already open')
    fopen(obj)
end

pause(0.5)
tic
ref = now;
while(obj.BytesAvailable ~= 0)
    parse = strsplit(fscanf(obj));            % delimit the string at whitespace
    if (length(parse)==3)
        if cell2mat(strfind(parse(1),'ACCX')) ~= 0
            axdata = circshift(axdata,1);
            ADC = str2num(cell2mat(parse(2)));
            V = (ADC/4095)*(Vp-Vm) + Vm;
            axdata(1) = (V-Vp/2)/0.3;
            ax.YData = axdata;
        elseif cell2mat(strfind(parse(1),'ACCY')) ~= 0
            aydata = circshift(aydata,1);
            ADC = str2num(cell2mat(parse(2)));
            V = (ADC/4095)*(Vp-Vm) + Vm;
            aydata(1) = (V-Vp/2)/0.3;
            ay.YData = aydata;
        elseif cell2mat(strfind(parse(1),'ACCZ')) ~= 0
            azdata = circshift(azdata,1);
            ADC = str2num(cell2mat(parse(2)));
            V = (ADC/4095)*(Vp-Vm) + Vm;
            azdata(1) = (V-Vp/2)/0.3;
            az.YData = azdata;
        elseif cell2mat(strfind(parse(1),'EDA')) ~= 0
            edadata = circshift(edadata,1);
            ADC = str2num(cell2mat(parse(2)));
            V = (ADC/4095)*(Vp-Vm) + Vm;
            edadata(1) = V;
            %edadatafiltered = filter(b,a,edadata);
            eda.YData = edadata;
        elseif cell2mat(strfind(parse(1),'PPG')) ~= 0
            ppgdata = circshift(ppgdata,1);
            ADC = str2num(cell2mat(parse(2)));
            V = (ADC/4095)*(Vp-Vm) + Vm;
            ppgdata(1) = V;
            ppg.YData = ppgdata;
        else
            % do nothing
        end
        drawnow limitrate
        HR = sum(diff(ppgdata(1:20/dt)>(mean(ppgdata(1:20/dt))*1.1))==1)*3
    end
end
