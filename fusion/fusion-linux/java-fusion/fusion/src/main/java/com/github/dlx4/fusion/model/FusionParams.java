package com.github.dlx4.fusion.model;

import lombok.Data;

@Data
public class FusionParams {

    private String imageUrlA;
    private String imageUrlB;
    private Double alpha;
    private Double beta;
    private Double sigma;
    private Double dalta;
    private Double level;
}
