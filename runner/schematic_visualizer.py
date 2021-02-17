import argparse
import pathlib
import tqdm

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation


def parse_args():
    parser = argparse.ArgumentParser()

    parser.add_argument("--detections",
                        required=True,
                        help="Path to the CSV file with detections",
                        type=pathlib.Path)
    return parser.parse_args()


def label_to_color(arr):
    return np.array(
        list(map(lambda x: '#1f77b4' if x == 'person' else '#ff7f0e', arr)))


def main():
    args = parse_args()

    df = pd.read_csv(args.detections)

    df['y_center'] = 800. * (df['ymin'] + df['height'] / 2.)
    df['x_center'] = 3840. * (df['xmin'] + df['width'] / 2.)

    start = 0
    end = df.iloc[df.shape[0] - 1].frame_num
    total = end - start

    fig, ax = plt.subplots(figsize=(12, 4))
    pbar = tqdm.tqdm(total=total)

    def frame(w):
        pbar.update(1)
        ax.clear()

        ax.set_ylim(-900.0, 100.0)
        ax.set_xlim(0.0, 3840.0)

        return ax.scatter(df[df.frame_num == (start + w)].x_center.values,
                          1 - df[df.frame_num == (start + w)].y_center.values,
                          c=label_to_color(
                              df[df.frame_num == (start + w)].label.values))

    anim = animation.FuncAnimation(fig,
                                   frame,
                                   frames=total,
                                   interval=40,
                                   blit=False,
                                   repeat=False)

    anim.save(args.detections.with_suffix(".mp4"))


if __name__ == "__main__":
    main()
