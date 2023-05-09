clear all;
clc;

% Block Size
N = 8;

% Quantizer Matrix for Lumenance Q50
QLum = [
     16  11  10  16  24  40  51  61;
     12  12  14  19  26  58  60  55;
     14  13  16  24  40  57  69  56;
     14  17  22  29  51  87  80  62;
     18  22  37  56  68 109 103  77;
     24  35  55  64  81 104 113  92;
     49  64  78  87 103 121 120 101;
     72  92  95  98 112 100 103  99
     ];

% Quantizer Matrix for Chromanence Q50
QChr = [
    17 18 24 47 99 99 99 99;
    18 21 26 66 99 99 99 99;
    24 26 56 99 99 99 99 99;
    47 66 99 99 99 99 99 99;
    99 99 99 99 99 99 99 99;
    99 99 99 99 99 99 99 99;
    99 99 99 99 99 99 99 99;
    99 99 99 99 99 99 99 99;
    ];

% Generate zigzag pattern for
zigzagG = zigzagCreate(N);

%%%%%%%%%%%%%%%%%
% Getting the image to the proper format to use the encoder
%%%%%%%%%%%%%%%%%

% Read image, convert to YCbCr
RGB = imread('Flooded_house.jpg');
heightOriginal = size(RGB, 1);
widthOriginal = size(RGB, 2);
BLANK = zeros(heightOriginal, widthOriginal, 'uint8'); % Blank canvas for writing new image
YCBCR = rgb2ycbcr(RGB);

% Split into YCbCr channels
Y = YCBCR(:,:,1);
Cb = YCBCR(:,:,2);
Cr = YCBCR(:,:,3);

% Subsample the Cb and Cr channels at 4:2:0
subCb = YCBCR(1:2:end,1:2:end,2);
subCr = YCBCR(1:2:end,1:2:end,3);

%%%%%%%%%%%%%%%%%%%%%
% Encoder
%%%%%%%%%%%%%%%%%%%%%
% Generate DCT Matrix
I = 1:N;
DCT = sqrt(2/N)*cos(((2*(I-1)+1).*(I(:)-1)*pi)/2/N);
DCT(1,:) = sqrt(1/N); 

% Cells were used so I could generalize the loop.
% Cells allow for matrices of different sizes.
channels = {Y, subCb, subCr};
qMatrices = {QLum, QChr, QChr};
% IDE complains I don't pre-allocate the cells below but idk how to do that
% when I don't know the final sizes of them ahead of time.
outputs = {};
for channelNum = 1:length(channels)
    % Grab the current channel out from the cell
    chan = cell2mat(channels(channelNum));
    % Grab current height/width, this may change in the future.
    chanHeight = size(chan,1);
    chanWidth = size(chan,2);
    % Convert image to doubles and shift to be centered around 0
    chanDShift = double(chan) - 128;
    % If image can't be subdivided evenly into 8x8 macroblocks, 
    % add reflected rows or columns to make it divisible by 8.
    % Reflection of the rows/columns can be used or replication of the
    % edges of rows/columns. It makes more sense to me to reflect given
    % the sinusoid. 
    % end-7+mod(chanHeight,8) calulates where we start the reflection.
    % Ex. If end is chanHeight/end is 34, then 34-7+34%8 = 27+2 = 29;
    %     So we start copying at 29 til 34, giving us 6 extra rows.
    %     Flip those rows, add them to the end 34+6=40 which divides by 8.
    if mod(chanHeight, 8) ~= 0
        chanDShift = [chanDShift; flip(chanDShift(end-7+mod(chanHeight,8):end,:),1)];
        chanHeight = size(chanDShift, 1);
    end
    if mod(chanWidth, 8) ~= 0
        chanDShift = [chanDShift flip(chanDShift(:, end-7+mod(chanWidth,8):end),2)];
        chanWidth = size(chanDShift, 2);
    end
    % Determine number of macroblocks
    numBlocksH = chanWidth/8;
    numBlocksV = chanHeight/8;
    % Pre-allocate the output matrix
    output = int8(zeros([numBlocksV*8 numBlocksH*8]));
    % Loop over each of the macroblocks
    for x=1:numBlocksH
        for y=1:numBlocksV
            % Calculate the offset for the current macroblock
            offsetV = 8*(y-1)+1;
            offsetH = 8*(x-1)+1;
            % Grab just the pixels for the current macroblock
            M = chanDShift(offsetV:offsetV+7,offsetH:offsetH+7);
            % Perform DCT on macroblock
            D = DCT*M*DCT';
            % Quantize the macroblock and round to nearest integer
            %Q = round(D./cell2mat(qMatrices(channelNum)));
            Q = round(D./cell2mat(qMatrices(channelNum)));
            % Save the quantized DCT'd macroblock to our output matrix.
            output(offsetV:offsetV+7,offsetH:offsetH+7) = Q;
            if y == 6 && (x == 0 || x == 1)  && channelNum == 1 
                disp(Q);
                imwrite(D, "DCT" + int2str(y) + int2str(x) + ".png");
                for i = 1:64
                    fprintf('%d ' ,round(Q(zigzagG(i, 1), zigzagG(i,2))));
                end
                disp("\n");
            end
        end
    end
    % Save complete matrix for the given channel back to the cells.
    outputs{channelNum} = output;
%    disp(min(output,[],'all'));
%    disp(max(output,[],'all'));
end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%Decoder
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
reconstChannels = {};
for channelNum = 1:length(channels)
   % Grab the current channel out from the cell
    chan = cell2mat(outputs(channelNum));
    % Grab current height/width, this may change in the future.
    chanHeight = size(chan, 1);
    chanWidth = size(chan, 2);
    % Pre-allocate output matrix
    reconstChannel = zeros(chanHeight, chanWidth, 'uint8');
    % Determine number of macroblocks
    numBlocksH = chanWidth/8;
    numBlocksV = chanHeight/8;
    % Loop over each of the macroblocks
    for x=1:numBlocksH
        for y=1:numBlocksV
            % Calculate the offset for the current macroblock
            offsetV = 8*(y-1)+1;
            offsetH = 8*(x-1)+1;
            % Grab just the pixels for the current macroblock
            Q = double(chan(offsetV:offsetV+7,offsetH:offsetH+7));
            % Quantize the macroblock and round to nearest integer
            D = Q.*cell2mat(qMatrices(channelNum));
            % Perform DCT on macroblock
            M = round(DCT'*D*DCT);
            % Put it back into the canvas
            reconstChannel(offsetV:offsetV+7,offsetH:offsetH+7) = M+128;
        end
    end
    reconstChannel = reconstChannel;
    reconstChannels{channelNum} = reconstChannel;
end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Upsample the Cb and Cr channels
% Code pulled mostly from HW1
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

reconstCb = cell2mat(reconstChannels(2));
reconstCr = cell2mat(reconstChannels(3));
% Trim off extra rows not in original
reconstCb = reconstCb(1:heightOriginal/2, 1:widthOriginal/2);
reconstCr = reconstCr(1:heightOriginal/2, 1:widthOriginal/2);

% Create blank matrices to draw the image.
linearCb = zeros(heightOriginal, widthOriginal);
linearCr = zeros(heightOriginal, widthOriginal);
reconstructedImage = zeros(heightOriginal, widthOriginal, 'uint8');
% From the subsampled matrices, distribute the pixels spaced at one apart
% on the output matrices. 
linearCb(1:2:end,1:2:end) = reconstCb(1:end, 1:end);
linearCr(1:2:end,1:2:end) = reconstCr(1:end, 1:end);

% If the number of rows (height) is even, the last row is duplicated when
% calculating the interpolated values. Otherwise, no issues with simple
% linear interpolation.
if mod(size(linearCb,1), 2) == 0
    linearCb(2:2:end, 1:2:end) = ...
        reconstCb(:, 1:1:end)/2 + ...
        [reconstCb(2:1:end,1:1:end);reconstCb(end,:)]/2;
    linearCr(2:2:end, 1:2:end) = ... 
        reconstCr(:,1:1:end)/2 + ...
        [reconstCr(2:1:end,1:1:end);reconstCr(end,:)]/2;
else
    linearCb(2:2:end, 1:2:end) = ...
        reconstCb(1:1:end-1,1:1:end)/2 + ...
        reconstCb(2:1:end,1:1:end)/2;
    linearCr(2:2:end, 1:2:end) = ... 
        reconstCr(1:1:end-1,1:1:end)/2 + ...
        reconstCr(2:1:end,1:1:end)/2;
end

% If the number of columns (width) is even, the last column is duplicated
% when calculating the interpolated values. Otherwise, no issues with
% simple linear interpolation.
if mod(size(linearCb,2),2) == 0
    linearCb(:,2:2:end) = ...
        linearCb(:,1:2:end)/2 + ...
        [linearCb(:,3:2:end) linearCb(:,end-1)]/2;
    linearCr(:,2:2:end) = ...
        linearCr(:,1:2:end)/2 + ...
        [linearCr(:,3:2:end) linearCr(:,end-1)]/2;
else
    linearCb(:,2:2:end) = ...
        linearCb(:,1:2:end-1)/2 + ...
        linearCb(:,3:2:end)/2;
    linearCr(:,2:2:end) = ...
        linearCr(:,1:2:end-1)/2 + ...
        linearCr(:,3:2:end)/2;
end

reconstY = cell2mat(reconstChannels(1));
RGBOUT = ycbcr2rgb(cat(3, reconstY, linearCb, linearCr));
imwrite(RGBOUT, "reconstructed.jpg");
imwrite(Y-reconstY, "difference.jpg");

% PSNR 
boop = psnr(reconstY, Y);


% Built-in macroblock and dct
% fun = @(block_struct) dct2(block_struct.data);
% D3 = blockproc(double(Y), [8 8], fun);       
        
% % Comparison between built-in dct and mine
% D2 = dct2(YSubDivMat);
% DD = abs(D-D2);
% DDS = sum(abs(D-D2),'all');
% % End comparison       

% Generate Zig Zag Pattern
function zigzag = zigzagCreate(N)
    row = 1;
    column = 1;
    dr = -1;
    dc = 1;
    length = 1;
    zigzag = zeros(64,2);
    while row <= N && column <= N
        %fprintf("(%d, %d), ", row, column);
        zigzag(length, :) = [row, column];
        length = length + 1;
        if row == 1
            if dr == -1, dr = 0;
            elseif dr == 0, dr = 1; dc = -1;
            end
        elseif row == N
            if dr == 1, dr = 0; dc = 1;
            elseif dr == 0, dr = -1;
            end
        elseif column == 1
            if dc == -1, dc = 0;
            elseif dc == 0, dc = 1; dr = -1;
            end
        elseif column == N
            if dc == 1, dc = 0; dr = 1;
            elseif dc == 0, dc = -1;
            end
        end
        row = row + dr;
        column = column + dc;
    end
    zigzag = uint8(zigzag);
end