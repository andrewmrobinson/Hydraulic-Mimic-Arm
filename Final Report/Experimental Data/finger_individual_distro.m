clear all
close all
clc
% I M R P
A = [1 1 0 0;...
    1 0 1 0;...
    1 0 0 1;...
    0 1 1 0;...
    0 1 0 1;...
    0 0 1 1;...
    1 1 1 0;...
    1 1 0 1;...
    1 0 1 1;...
    0 1 1 1;...
    1 1 1 1];
X = [ 3.5 4 5 2 2 4 5 5 6 5 8]';

inv(A'*A)*A'*X