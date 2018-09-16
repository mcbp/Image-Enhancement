# Image-Enhancement

A university image enhancement project where OpenCV was used to manipulate images in their spatial and frequency domains in order to restore them.

## Original image
This image of a penguin can be seen to contain both traces of periodic noise and random noise.
A DFT of the image has been taken using OpenCV to highlight the low and high frequencies.

<img src="/OpenCV3_Template/PenguinNoise.jpg"><img src="/OpenCV3_Template/PenguinDFT.jpg">

## Random noise removed
To remove the random noise the pixels that are of the high and low intensities targeted, this is done by searching the image for intensity values above or below a set threshold.
Once these pixels are selected a filter can be applied that sets them to an average of their neighbouring pixels. 

<img src="/OpenCV3_Template/penguinNoiseFilter.jpg"><img src="/OpenCV3_Template/PenguinNoiseRemovalDFT.jpg">

## Gaussian blur
A Gaussian blur is applied to soften the image, removing traces of the random noise but at the cost of some loss of detail.

<img src="/OpenCV3_Template/penguinGaussianBlur.jpg"><img src="/OpenCV3_Template/penguinGaussianDFT.jpg">

## Bilateral filter
A bilateral filter is similar to the Gaussin blur however edges are detevted which prevents smoothing across large and adjancent intensity changes.

<img src="/OpenCV3_Template/penguinBilateralFilter5x5.jpg"><img src="/OpenCV3_Template/penguinBilateralDFT5x5.jpg">

