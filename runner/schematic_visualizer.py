import argparse
import pathlib
import tqdm

import scipy.optimize
import scipy.spatial.distance

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


def get_step(df, step_ix, label):
    return df[(df.frame_num == step_ix)
              & (df.label == label)][["y_center", "x_center"]].values


def main():
    args = parse_args()

    df = pd.read_csv(args.detections)

    df['y_center'] = 800. * (1. - df['ymin'] + df['height'] / 2.)
    df['x_center'] = 3840. * (df['xmin'] + df['width'] / 2.)

    start = 0
    end = df.iloc[df.shape[0] - 1].frame_num
    total = end - start

    fig, ax = plt.subplots(figsize=(12, 4))
    pbar = tqdm.tqdm(total=total)

    def frame(w):
        pbar.update(1)
        ax.clear()

        ax.set_ylim(-100.0, 900.0)
        ax.set_xlim(0.0, 3840.0)

        prev_person_corrds = get_step(df, start + w, 'person')
        current_person_coords = get_step(df, start + w + 1, 'person')
        cost_mat = scipy.spatial.distance.cdist(prev_person_corrds,
                                                current_person_coords)
        cols, rows = scipy.optimize.linear_sum_assignment(cost_mat)
        deltas = current_person_coords[rows] - prev_person_corrds[cols]
        deltas = deltas[(np.linalg.norm(deltas, axis=1) < 15)
                        & (np.linalg.norm(deltas, axis=1) > 4)]

        xs = prev_person_corrds[:, 1]
        ys = prev_person_corrds[:, 0]

        ax.scatter(df[df.frame_num == (start + w)].x_center.values,
                   df[df.frame_num == (start + w)].y_center.values,
                   c=label_to_color(df[df.frame_num == (start +
                                                        w)].label.values))

        dy, dx = np.sum(deltas, axis=0)
        return ax.arrow(3000, 300, dx, dy, width=10)

    anim = animation.FuncAnimation(fig,
                                   frame,
                                   frames=total - 1,
                                   interval=40,
                                   blit=False,
                                   repeat=False)

    anim.save(args.detections.with_suffix(".mp4"))


if __name__ == "__main__":
    main()
