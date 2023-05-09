%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% CompE565 Homework 3
% April 03, 2022
% Name: Christopher Kihano, Jarel-John Macanas
% RedID: 842744811, 822433138
% email: ckihano2017@sdsu.edu, jmacanas4806@sdsu.edu
%
% Instructions: 
% HW3 can be run completely from this file. Just hit Run.
% A folder called "output" will appear in the directory this script is run
% from with any of the outputs this script  creates. The outputs are
% - estimated frames for frames 11-14
% - difference/error frames for frames 11-14
% - motion vectors for frames 11-14
% - original input frames 10-14
% - reproduced frames 11-14
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clc;
clear all;

video = VideoReader('football_qcif.avi');
macroBlockSize = 16;
p = 8;
startFrame = 10;
numOfPFrames = 4;
numOfHMacroBlocks = video.Width/macroBlockSize;
numOfVMacroBlocks = video.Height/macroBlockSize;
% Pull the number of wanted frames from the video
framesRGB = read(video,[startFrame startFrame + numOfPFrames]);
framesYCbCr = zeros(video.Height, video.Width, 3, 5);
% Convert to YCbCr
for i=1:5 
    framesYCbCr(:, :, :,i) = rgb2ycbcr(framesRGB(:, :, :, i));
end
% Just use the Y channel
frames = int32(reshape(framesYCbCr(:, :, 1, :), video.Height, video.Width, 5));

% Create an empty 4D matrix to hold the motion vectors
% (4 p frames after i frame, # of vertical macroblocks, # of horizontal macroblocks, x and y offsets)
mvectors = zeros(numOfPFrames, numOfVMacroBlocks, numOfHMacroBlocks, 2);
% Estimated frames based on macroblock estimation, does not include residual
estimated = int32(zeros(video.Height, video.Width, numOfPFrames));
% Error between estimated frames and the actual frame. Can be compressed
residuals = int32(zeros(video.Height, video.Width, numOfPFrames));
% Reconstructed frames from estimated and residual.
reconstructed = int32(zeros(video.Height, video.Width, numOfPFrames+1));
% Set first frame of reconstructed frames to the original frame, makes
% loops easier to work with.
reconstructed(:, :, 1) = frames(:, :, 1);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Create Motion Vectors
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Skip the first frame, increase the p-frame count by 1 to take into
% account the i-frame.
for i = 2:numOfPFrames+1
    % Scan through macroblocks
    for x = 1:numOfHMacroBlocks
        for y = 1:numOfVMacroBlocks
            % Calculate the offset for the current macroblock
            offsetV = macroBlockSize*(y-1)+1;
            offsetH = macroBlockSize*(x-1)+1;
            % We're looking for a block in the previous frame similar to the macroblock in the current frame.
            curMacro = frames(offsetV:offsetV+macroBlockSize-1,offsetH:offsetH+macroBlockSize-1,i);
            % We want to find a block that has a min difference from the reference macroblock. Set initial difference to infinity.
            minDiff = inf;
            % scan 8 pixels up and 8 down, total window size of 16+8+8=32
            for dy = -p:p
                % scan 8 pixels left and 8 right
                for dx = -p:p
                    top = offsetV+dy;
                    bottom = offsetV+dy+macroBlockSize;
                    left = offsetH+dx;
                    right = offsetH+dx+macroBlockSize;
                    % Check if we're within the bounds of the video frame
                    if(top > 0 && bottom <= video.Height && left > 0 && right <= video.Width)
                        % Get current block in the previous frame to compare against the current frame macroblock
                        curBlock = reconstructed(top:bottom-1,left:right-1,i-1);
                        % Use sum absolute difference on all channels
                        curDiff = abs(sum(curMacro-curBlock, 'all'));
                        % If we found a better candidate, save its location and update the new minimum difference
                        if minDiff > curDiff
                            minDiff = curDiff;
                            mvectors(i-1, y, x, :) = [dy,dx];
                        end
                    end
                end
            end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Create Frame Based on Estimations
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            blockOffsetV = offsetV+mvectors(i-1,y,x,1);
            blockOffsetH = offsetH+mvectors(i-1,y,x,2);
            estimated(offsetV:offsetV+macroBlockSize-1,offsetH:offsetH+macroBlockSize-1, i-1) = ...
                frames(blockOffsetV:blockOffsetV+macroBlockSize-1, blockOffsetH:blockOffsetH+macroBlockSize-1, i-1);
        end
    end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Create Residual Frames
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Residuals store the full frame of pixels but only the differences between the motion vectors and the actual frame.
% This can be highly compressed. In this lab, we don't compress them but in HW4 we do. 
    residuals(:, :, i-1) = frames(:, :, i) - estimated(:, :, i-1);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Create Reconstructed Frames
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Since we did not do any compression on the residuals, these should be identical to the source frames.
    reconstructed(:, :, i) = estimated(:, :, i-1) + residuals(:, :, i-1);
end


% % Save reproduced frames, estimated frames, residual frames, and motion vectors
warning('off', 'MATLAB:MKDIR:DirectoryExists');
status = mkdir("outputs");
% Turn off displaying of figures.
figure('Visible','off');
for i = 1:numOfPFrames
    % Don't forget to re-eanble figure display
    %figure(i);
%     subplot(3,2,1);
%     imshow(uint8(frames(:,:,i)));
%     subplot(3,2,2);
%     imshow(uint8(frames(:,:,i+1)));
%     subplot(3,2,[3 4 5 6]); 
    quiver(reshape(mvectors(i,:,:,2),9,11),reshape(mvectors(i,:,:,1),9,11));
    axis([0 numOfHMacroBlocks+1 0 numOfVMacroBlocks+1]);
    img = gca;
    exportgraphics(img, "outputs/motionVectors"+ (startFrame+i) + ".png", 'Resolution',300);
    bmin = min(residuals(:,:,i), [], 'all');
    amax = max(frames(:,:,i+1), [], 'all');
    residual = (residuals(:,:,i)-bmin)*255/(amax-bmin);
    imwrite(imresize(uint8(residual),4, 'nearest'), "outputs/residualframe" + (startFrame+i) + ".png");
    imwrite(imresize(uint8(estimated(:,:,i)),4, 'nearest'), "outputs/estimatedframe" + (startFrame+i) + ".png");
    imwrite(imresize(uint8(reconstructed(:,:,i)), 4, 'nearest'), "outputs/reproducedframe" + (startFrame + i) + ".png", "png");
end

% % Write input frames
for i = 1:numOfPFrames+1
    imwrite(imresize(uint8(frames(:,:,i)),4,'nearest'), "outputs/inputframe" + (startFrame + i - 1) + ".png", "png");
end