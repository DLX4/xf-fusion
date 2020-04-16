package com.github.dlx4.fusion.controller;

import com.github.dlx4.fusion.model.FusionParams;
import com.github.dlx4.fusion.utils.FileNameUtils;
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

import java.util.HashMap;
import java.util.Map;

@Controller
public class FusionController {

    private final ResourceLoader resourceLoader;
    @Value("${web.upload-path}")
    private String path;

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

        String fileName = FileNameUtils.generateRandomFileName(file.getOriginalFilename());
        String fullPath = path + "/" + fileName;
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
     *
     * @return
     */
    @RequestMapping("/fusion")
    public ResponseEntity fusion(@RequestBody FusionParams params) {

        try {
            // 由于是读取本机的文件，file是一定要加上的， path是在application配置文件中的路径
            return ResponseEntity.ok(resourceLoader.getResource("file:" + path + "2dd0f45ae0df40eaa2debe27c907ec62.PNG"));
        } catch (Exception e) {
            return ResponseEntity.notFound().build();
        }
    }

}
