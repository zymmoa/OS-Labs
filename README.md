# RRNet
* Results:
  - We provide the resutls of our RRNet on EORSSD. 
```
```
* Pretrained model:
  - We provide our testing code. If you test our model, please download the pretrained model, unzip it, and put the checkpoint `RRNet_pretrained.pth` to `Checkpoints/trained/` folder 
  and put the pretrained backbone `backbone_r.pth` to `Checkpoints/warehouse/` folder.
  - Pretrained model download:
```

```

# Pytorch
* Pytorch implementation of RRNet

## Requirements

* Python 3.7
* Pytorch 1.5.1
* torchvision

## Data Preprocessing
* We resize the images of original EORSSD dataset. For your evaluations, we also provide the corresponding resized images and labels. 
If you test our model, please download the resized data, and put the data to `train_and_test/` folder.
* Resized EORSSD:
```
```

## Test
```
python test.py
```

* You can find the results in the `'Outputs/Outputs_GR'` folder.
* You can use a script to resize the results back to the same size as the original RGB-D image,  or just use the results with a size of 224*224 for evaluations. 
We did not find much differences for the evaluation results.

# Bibtex
If you use the results and code, please cite our paper.
```
@article{RRNet,
  title={{RRNet}: Relational Reasoning Network with Parallel Multi-scale Attention for Salient Object Detection in Optical Remote Sensing Images},
  author={Cong, Runmin and Zhang, Yumo and Fang, Leyuan and Li, Jun and Zhang, Chunjie and Zhao, Yao and Kwong, Sam},
  journal={IEEE Transactions on Geoscience and Remote Sensing},
  year={2021},
  publisher={IEEE}
}
```
