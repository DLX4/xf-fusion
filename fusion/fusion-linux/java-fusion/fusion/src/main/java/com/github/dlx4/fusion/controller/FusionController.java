package com.github.dlx4.fusion.controller;

import com.github.dlx4.fusion.ftp.FusionImageFtpClient;
import com.github.dlx4.fusion.model.FusionParams;
import com.github.dlx4.fusion.utils.FileUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.core.io.ResourceLoader;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.multipart.MultipartFile;

import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

@Controller
public class FusionController {

    private final ResourceLoader resourceLoader;
    @Value("${web.upload-path}")
    private String path;

    @Value("${ftp.server.host}")
    private String ftpHost;

    @Value("${ftp.server.port}")
    private String ftpPort;

    @Value("${ftp.server.username}")
    private String ftpUsername;

    @Value("${ftp.server.password}")
    private String ftpPassword;

    @Value("${ftp.server.source-directory}")
    private String sourceDirectory;

    @Value("${ftp.server.fusion-directory}")
    private String fusionDirectory;

    @Value("${ftp.server.config-directory}")
    private String configDirectory;

    @Autowired
    public FusionController(ResourceLoader resourceLoader) {
        this.resourceLoader = resourceLoader;
    }

    /**
     * @param file 要上传的文件
     * @return
     */
    @RequestMapping("/fileUpload")
    public ResponseEntity uploadImage(@RequestParam("image") MultipartFile file) {

        Map<String, Object> map = new HashMap<>();
        // 上传成功或者失败的提示
        String msg = "";

        String fileName = FileUtils.generateRandomFileName(file.getOriginalFilename());
        String fullPath = path + fileName;
        if (FileUtils.upload(file, fullPath)) {
            // 上传成功，给出页面提示
            msg = "上传成功！";
        } else {
            msg = "上传失败！";
        }

        // 显示图片
        map.put("msg", msg);
        map.put("fileName", fileName);

        return ResponseEntity.ok(map);
    }

    /**
     * 显示单张图片
     *
     * @return
     */
    @RequestMapping("/show")
    public ResponseEntity showImage(String fileName) {

        try {
            // 由于是读取本机的文件，file是一定要加上的， path是在application配置文件中的路径
            return ResponseEntity.ok(resourceLoader.getResource("file:" + path + fileName));
        } catch (Exception e) {
            return ResponseEntity.notFound().build();
        }
    }

    /**
     * 融合
     * @return
     */
    @RequestMapping("/fusion")
    public ResponseEntity fusion(@RequestBody FusionParams params) throws IOException, InterruptedException {

        // 目标融合图像文件
        // TODO
        // params.setImageFusion(FileUtils.generateRandomFileName("fusion.jpg"));
        params.setImageFusion("123.jpg");
        params.setConfig(FileUtils.generateRandomFileName("fusion.config"));
        FusionImageFtpClient fusionImageFtpClient = new FusionImageFtpClient(
                ftpHost,
                ftpPort,
                ftpUsername,
                ftpPassword,
                sourceDirectory,
                fusionDirectory,
                configDirectory,
                path,
                params
                );

        fusionImageFtpClient.doFusion();
        fusionImageFtpClient.close();

        Map<String, Object> map = new HashMap<>();
        // 显示图片
        map.put("msg", "OK");
        map.put("fileName", params.getImageFusion());

        return ResponseEntity.ok(map);
    }
}
