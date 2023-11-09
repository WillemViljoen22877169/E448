% Copyright 2012 The MathWorks, Inc.
numCells = 4; % Updated number of cells to 4
% load('Kokam_LUT_3RC')%
results.T5C  = load('batteryParameterEstimation_results_3RC_5degC.mat');
results.T20C = load('batteryParameterEstimation_results_3RC_20degC.mat');
results.T40C = load('batteryParameterEstimation_results_3RC_40degC.mat');
SOC_LUT = results.T5C.SOC_LUT;

%% Thermal Properties
% Cell dimensions and sizes
cell_thickness = 0.018; %m
cell_width = 0.018; %m
cell_height = 0.0650; %m

% Cell surface area
cell_area = 2 * (...
    cell_thickness * cell_width +...
    cell_thickness * cell_height +...
    cell_width * cell_height); %m^2

% Cell volume
cell_volume = cell_thickness * cell_width * cell_height; %m^3

for idx = 1:numCells
%% Lookup Table Breakpoints
Battery(idx).SOC_LUT = SOC_LUT;
Battery(idx).Temperature_LUT = [5 20 40] + 273.15;

%% Em Branch Properties (OCV, Capacity)
Battery(idx).Capacity_LUT = [ 4 4 4]; %Ampere*hours
Battery(idx).Em_LUT = [ results.T5C.Em ...
                        results.T20C.Em ...
                        results.T40C.Em]; %Volts
                        
%% Terminal Resistance Properties
Battery(idx).R0_LUT = [ results.T5C.R0 ...
                        results.T20C.R0 ...
                        results.T40C.R0]; %Ohms

%% RC Branch 1 Properties
Battery(idx).R1_LUT = [ results.T5C.R1 ...
                        results.T20C.R1 ...
                        results.T40C.R1]; %Ohms
Battery(idx).R2_LUT = [ results.T5C.R2 ...
                        results.T20C.R2 ...
                        results.T40C.R2]; %Ohms
Battery(idx).R3_LUT = [ results.T5C.R3 ...
                        results.T20C.R3 ...
                        results.T40C.R3]; %Ohms
Battery(idx).t1_LUT = [ results.T5C.tau1 ...
                        results.T20C.tau1 ...
                        results.T40C.tau1]; %Farads
Battery(idx).t2_LUT = [ results.T5C.tau2 ...
                        results.T20C.tau2 ...
                        results.T40C.tau2]; %Farads
Battery(idx).t3_LUT = [ results.T5C.tau3 ...
                        results.T20C.tau3 ...
                        results.T40C.tau3]; %Farads

%% Thermal Properties
Battery(idx).cell_mass = 0.05; %kg
Battery(idx).cell_rho_Cp = 2.04E6; %J/m3/K
Battery(idx).cell_Cp_heat = Battery(idx).cell_rho_Cp * cell_volume / Battery(idx).cell_mass; %J/kg/K

%% Initial Conditions
Battery(idx).T_init = 20 + 273.15; %K
end

%% Initial charge deficit
Battery(1).SOC0 = 0.2;
Battery(2).SOC0 = 0.21; %Ampere*hours
Battery(3).SOC0 = 0.22; %Ampere*hours
Battery(4).SOC0 = 0.23; % Added initial charge deficit for the fourth cell

% Convective heat transfer coefficient
h_conv = 5; %W/m^2/K Cell-to-cell
h_conv_end = 10; %W/m^2/K End cells to ambient

%% Passive balancing parameters
I_cc = 4;
R = .2;
R_bleed = 20;
T_volt = 600;
V_th = 11.5;
deltaV = 0.01;
V_top = 4.1;

%% PID Controller Gains
P_gain = 100;
I_gain = 10;
