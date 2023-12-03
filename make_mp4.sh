#!/bin/bash
ffmpeg -framerate 30 -i output/output%04d.png -s:v 800x600 -c:v libx264 -profile:v high -crf 28 -pix_fmt yuv420p voronoi.mp4
