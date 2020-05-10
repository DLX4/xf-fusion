package com.github.dlx4.fusion.model;

import lombok.Data;

/**
 * 融合参数
 * @author dinglingxiang
 */
@Data
public class FusionParams {

    private String imageA;
    private String imageB;
    private Double alpha;
    private Double beta;
    private Double sigma;
    private Double delta;
    private Double level;

    /** 融合好的图片文件 **/
    private String imageFusion;
    /** 融合任务文件 **/
    private String config;

    public FusionParams() {}

    public FusionParams(String imageA, String imageB) {
        this.imageA = imageA;
        this.imageB = imageB;
    }

    @Override
    public String toString() {
        return "imageA=" + imageA + "\r\n" +
                "imageB=" + imageB + "\r\n" +
                "alpha=" + alpha + "\r\n" +
                "beta=" + beta + "\r\n" +
                "sigma=" + sigma + "\r\n" +
                "delta=" + delta + "\r\n" +
                "level=" + level + "\r\n" +
                "imageFusion=" + imageFusion + "\r\n" +
                "config=" + config + "\r\n" ;
    }
}
