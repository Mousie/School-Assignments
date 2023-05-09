%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% CompE565 Homework 1
% Feb 10, 2022
% Name: Christopher Kihano, Jarel-John Macanas
% RedID: 842744811, 822433138
% email: ckihano2017@sdsu.edu, jmacanas4806@sdsu.edu
%
% Instructions: 
% HW1 can be run completely from this file. Just hit Run.
% The script will open 7 windows containing figures for each of the steps
% which requires us to display output. Each of the images within the
% figures are labeled.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear all;
clc;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Problem 1: Read and display the image using Matlab
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
disp("Opening image file and displaying it.");

RGB = imread('Flooded_house.jpg');
height = size(RGB, 1);
width = size(RGB, 2);
figure(1);
subplot(1, 1, 1);
imshow(RGB);
title("Original RGB Image");

disp("Done with opening and showing image.");

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Problem 2: Display each band (Red, Green, and Blue) of the image file
% Location of output image: output0201Red.jpg 
%                           output0202Blue.jpg 
%                           output0203Green.jpg
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
disp("Displaying the 3 channels, RGB, of the input image.")

% Create a matrix of all zeros from the dimensions of the input image.
% This will be used for a few of the following steps as well.
BLANK = zeros(height, width, 'uint8');

% Red channel
figure(2);
subplot(1,3,1);
RED = cat(3, RGB(:,:,1), BLANK, BLANK);
imshow(RED);
title("Red Channel");
imwrite(RED, "output0201Red.jpg");

% Green channel
subplot(1,3,2);
GREEN = cat(3, BLANK, RGB(:,:,2), BLANK);
imshow(GREEN);
title("Green Channel");
imwrite(GREEN, "output0202Green.jpg");

 % Blue channel
subplot(1,3,3);
BLUE = cat(3, BLANK, BLANK, RGB(:,:,3));
imshow(BLUE);
title("Blue Channel");
imwrite(BLUE, "output0203Blue.jpg");

disp("Done with the the color bands.");

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Problem 3: Convert the image into YCbCr color space
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
disp("Converting the RGB channels into YCbCr.");

YCBCR = rgb2ycbcr(RGB);

disp("Conversion complete.");

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Problem 4: Display each band seperately (Y, Cb, and Cr bands)
% Location of output image: output0401Y.jpg
%                           output0402Cb.jpg
%                           output0403Cr.jpg 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
disp("Displaying the YCbCr channels");

figure(3);
% Split out the Y, Cb, and Cr channels.

% Y Channel
subplot(1,3,1);
Y = YCBCR(:,:,1);
imshow(Y);
title("Y Channel");
imwrite(Y, "output0401Y.jpg");

% Cb channel
subplot(1,3, 2);
Cb = YCBCR(:,:,2);
imshow(Cb);
title("Cb Channel");
imwrite(Cb, "output0402Cb.jpg");

% Cr channnel
subplot(1,3,3);
Cr = YCBCR(:,:,3);
imshow(Cr);
title("Cr Channel");
imwrite(Cr, "output0403Cr.jpg");

disp("Successfully displayed.");

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Problem 5: Subsample Cb and Cr bands using 4:2:0 and display both bands
% Location of output image: output0501SubCr.jpg
%                           output0502SubCb.jpg
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
disp("Subsampling Cb and Cr channels using 4:2:0");

% Get the Cb and Cr channels
subCb = YCBCR(:,:,2);
subCr = YCBCR(:,:,3);

% Remove every other pixel in both the vertical and horizontal axes.
subCb = subCb(1:2:end, 1:2:end);
subCr = subCr(1:2:end, 1:2:end);

% Display and output
figure(4);
subplot(1, 2, 1);
imshow(subCb);
title("4:2:0 Cb Channel");
imwrite(subCb, "output0501SubCb.jpg");

subplot(1, 2, 2);
imshow(subCr);
title("4:2:0 Cr Channel");
imwrite(subCr, "output0502SubCr.jpg");

disp("Subsampling Complete");

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Problem 6, Part 1: Upsample and display the Cb and Cr bands using linear
% interpolation.
% Location of output image: output0601LinearCb.jpg
%                           output0602LinearCr.jpg
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
disp("Upsampling using linear interpolation");

% Create blank matrices to draw the image.
linearCb = BLANK;
linearCr = BLANK;

% From the subsampled matrices, distribute the pixels spaced at one apart
% on the output matrices. 
linearCb(1:2:end,1:2:end) = subCb;
linearCr(1:2:end,1:2:end) = subCr;

% If the number of rows (height) is even, the last row is duplicated when
% calculating the interpolated values. Otherwise, no issues with simple
% linear interpolation.
if mod(height, 2) == 0
    linearCb(2:2:end, 1:2:end) = ...
        subCb(:, 1:1:end)/2 + ...
        [subCb(2:1:end,1:1:end);subCb(end,:)]/2;
    linearCr(2:2:end, 1:2:end) = ... 
        subCr(:,1:1:end)/2 + ...
        [subCr(2:1:end,1:1:end);subCr(end,:)]/2;
else
    linearCb(2:2:end, 1:2:end) = ...
        subCb(1:1:end-1,1:1:end)/2 + ...
        subCb(2:1:end,1:1:end)/2;
    linearCr(2:2:end, 1:2:end) = ... 
        subCr(1:1:end-1,1:1:end)/2 + ...
        subCr(2:1:end,1:1:end)/2;
end

% If the number of columns (width) is even, the last column is duplicated
% when calculating the interpolated values. Otherwise, no issues with
% simple linear interpolation.
if mod(width,2) == 0
    linearCb(:,2:2:end) = ...
        linearCb(:,1:2:end)/2 + ...
        [linearCb(:,3:2:end) linearCb(:,end)]/2;
    linearCr(:,2:2:end) = ...
        linearCr(:,1:2:end)/2 + ...
        [linearCr(:,3:2:end) linearCr(:,end)]/2;
else
    linearCb(:,2:2:end) = ...
        linearCb(:,1:2:end-1)/2 + ...
        linearCb(:,3:2:end)/2;
    linearCr(:,2:2:end) = ...
        linearCr(:,1:2:end-1)/2 + ...
        linearCr(:,3:2:end)/2;
end

% Display and output
figure(5);
subplot(1, 2, 1);
imshow(linearCb);
title(["Upsampled Linear", "Interpolated", "Cb Channel"]);
imwrite(linearCb, "output0601LinearCb.jpg");

subplot(1, 2, 2);
imshow(linearCr);
title(["Upsampled Linear", "Interpolated", "Cr Channel"]);
imwrite(linearCr, "output0602LinearCr.jpg");

disp("Completed linear interpolation step");

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Problem 6, Part 2: Upsample and display the Cb and Cr bands using simple
% row or column replication.
% Location of output image: output0603ReplicationCb.jpg
%                           output0604ReplicationCr.jpg
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
disp("Upsampling using replication");

% Create blank matrices to hold canvas
replicationCb = BLANK;
replicationCr = BLANK;

% From the subsampled matrices, distribute the pixels spaced at one apart
% on the output matrices. 
replicationCb(1:2:end,1:2:end) = subCb;
replicationCr(1:2:end,1:2:end) = subCr;

% If the number of rows (height) is even, the last row is duplicated when
% calculating the interpolated values. Otherwise, no issues with simple
% replication.
if mod(height, 2) == 0
    replicationCb(2:2:end,1:1:end) = replicationCb(1:2:end,1:1:end);
    replicationCr(2:2:end,1:1:end) = replicationCr(1:2:end,1:1:end);
else
    replicationCb(2:2:end,1:1:end) = replicationCb(1:2:end-1,1:1:end);
    replicationCr(2:2:end,1:1:end) = replicationCr(1:2:end-1,1:1:end);
end
% If the number of columns (width) is even, the last column is duplicated
% when calculating the interpolated values. Otherwise, no issues with
% simple replication.
if mod(width, 2) == 0
    replicationCb(:,2:2:end) = replicationCb(:,1:2:end);
    replicationCr(:,2:2:end) = replicationCr(:,1:2:end);
else
    replicationCb(:,2:2:end) = replicationCb(:,1:2:end-1);
    replicationCr(:,2:2:end) = replicationCr(:,1:2:end-1);
end

% Display and save output
figure(6);
subplot(1, 2, 1);
imshow(replicationCb);
title(["Upsampled Replicated", "Cb Channel"]);
imwrite(replicationCb, "output0603ReplicationCb.jpg");

subplot(1, 2, 2);
imshow(replicationCr);
title(["Upsampled Replicated", "Cr Channel"]);
imwrite(replicationCr, "output0604ReplicationCr.jpg");

disp("Completed replication");
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Problem 7: Convert the image into RGB format
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
disp("Converting restored Cb and Cr channels into RGB format");

interpolated = ycbcr2rgb(cat(3, Y, linearCb, linearCr));
replicated = ycbcr2rgb(cat(3, Y, replicationCb, replicationCr));

disp("Conversion complete");
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Problem 8: Display the original and reconstructed images (the image
% restored from the YCbCr coordinate)
% Location of output image: output0801Interpolated.jpg
%                           output0802Replicated.jpg
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
disp("Displaying original and reconstructed images");
figure(7);
subplot(1, 3, 1);
imshow(RGB);
title("Original Image");

subplot(1, 3, 2);
imshow(interpolated);
title(["Upsampled Linear", "Interpolation 4:2:0"]);
imwrite(interpolated, "output0801Interpolated.jpg");

subplot(1, 3, 3);
imshow(replicated);
title(["Upsampled Replicated", "4:2:0"]);
imwrite(replicated, "output0801Replicated.jpg");

disp("Display complete");
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Problem 9: Comment on the visual quality of the reconstructed image for
% both upsampling cases. 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
disp("The image looks mostly the same with areas, typically around" + ...
    " edges showing distortion.");

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Problem 10: Measure MSE between the original and reconstructed images
% (obtained using linear interpolation only). Comment on the results.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
disp("Calculating MSE for the linearly interpolated reconstructed image");

MSEinterp = sum((int16(RGB) - int16(interpolated)).^2, 'all')/width/height;

disp("Total MSE for the given image was ");
disp(MSEinterp);
disp("Average MSE between the three channels for the given image was");
disp(MSEinterp/3);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Problem 11: Comment on the compression ratio acheived by the subsampling
% Cb and Cr components for 4:2:0 approach. Please not that you do not send
% the pixels which are made zero in the row and columns during subsampling.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
disp("Calculating Compression Ratio");

numEl444 = numel(Y) + numel(Cb) + numel(Cr);
numEl420 = numel(Y) + numel(subCb) + numel(subCr);
ratio = numEl420/numEl444*100;

disp("The compression ratio for this image was ");
disp(ratio*0.01);