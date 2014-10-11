#!/usr/bin/octave --persist

%% profiler.m
%% Plots the temperature profile of a amplifier using dmmlog's output.
%%
%% Author: Nathan Campos <nathanpc@dreamintech.net>

function plot_profile(filename, csv)
	% TODO: Plot the difference of them (to know if the heatsink datasheet is right.)
	plot(csv(:, 1), csv(:, 3), ";Heatsink;", csv(:, 1), csv(:, 4), ";Ambient;");

	% Setup the plot design/layout.
	grid();
	xlabel("Time (s)");
	ylabel("Temperature (C)");
	title(filename);
endfunction

% Read file and plot the data.
filename = argv(){1};
csv = csvread(filename);
plot_profile(filename, csv);

