package com.github.dlx4.fusion.ftp;

import com.github.dlx4.fusion.model.FusionParams;
import com.github.dlx4.fusion.utils.FileUtils;

import java.io.File;
import java.io.IOException;
import java.util.Collection;

/**
 * 通过ftp和图像融合服务器（在本项目中是开发板）交互
 * @author dinglingxiang  2020/4/17 11:18
 */
public class FusionImageFtpClient {

    private final FtpClient ftpClient;

    /** 服务器端存放融合源文件的目录 **/
    private final String remoteSourceDirectory;
    /** 服务器端存放融合完成文件的目录 **/
    private final String remoteFusionDirectory;
    /** 服务器端存放融合参数文件的目录 **/
    private final String remoteConfigDirectory;

    /** 本地存放图片的目录 **/
    private final String localDirectory;

    private final FusionParams params;

    public FusionImageFtpClient(
            String ftpHost,
            String ftpPort,
            String ftpUsername,
            String ftpPassword,
            String sourceDirectory,
            String fusionDirectory,
            String configDirectory,
            String localDirectory,
            FusionParams params
    ) throws IOException {
        this.remoteSourceDirectory = sourceDirectory;
        this.remoteFusionDirectory = fusionDirectory;
        this.remoteConfigDirectory = configDirectory;
        this.localDirectory = localDirectory;
        this.params = params;
        ftpClient = new FtpClient(ftpHost, Integer.parseInt(ftpPort), ftpUsername, ftpPassword);
        ftpClient.open();
    }

    /**
     * 下载融合图像到本地
     */
    private void download() throws IOException {
        ftpClient.downloadFile(remoteFusionDirectory + params.getImageFusion(), localDirectory + params.getImageFusion());
    }

    /**
     * 上传文件
     */
    private void uploadSourceImage() throws IOException {
        ftpClient.putFileToPath(new File(localDirectory + params.getImageA()), remoteSourceDirectory + params.getImageA());
        ftpClient.putFileToPath(new File(localDirectory + params.getImageB()), remoteSourceDirectory + params.getImageB());
    }

    /**
     * 上传融合任务配置文件（将会触发对端执行融合动作）
     * @throws IOException
     */
    private void uploadFusionConfig() throws IOException {
        FileUtils.writeFile(localDirectory + params.getConfig(), params.toString());
        ftpClient.putFileToPath(new File(localDirectory + params.getConfig()), remoteConfigDirectory + params.getConfig());
    }

    /**
     * 轮询config文件是否被删除
     * 如果轮询不到则去下载融合图像到本地，并返回true
     * 否则返回false
     */
    private boolean poll() throws IOException {
        Collection<String> files = ftpClient.listFiles(remoteConfigDirectory);

        if (!files.contains(params.getConfig())) {
            download();
            return true;
        } else {
            return false;
        }
    }

    /**
     * 处理融合
     */
    public void doFusion() throws IOException, InterruptedException {
        // 上传融合输入文件
        uploadSourceImage();
        // 上传融合任务config文件
        uploadFusionConfig();
        // 轮询融合任务是否完成
        while (!poll()) {
            // 等一等
            Thread.sleep(1000);
        }
        // 下载融合输出图像
        download();
    }

    /**
     * 关闭连接
     * @param
     * @return void
     */
    public void close() throws IOException {
        ftpClient.close();
    }
}
