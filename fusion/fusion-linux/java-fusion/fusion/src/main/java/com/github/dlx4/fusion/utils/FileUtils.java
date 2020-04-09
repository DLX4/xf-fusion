package com.github.dlx4.fusion.utils;

import org.springframework.web.multipart.MultipartFile;

import java.io.File;
import java.io.IOException;

/**
 * 文件上传工具包
 */
public class FileUtils {

    /**
     * @param file     文件
     * @param fullPath 文件存放路径
     * @return
     */
    public static boolean upload(MultipartFile file, String fullPath) {

        File dest = new File(fullPath);

        // 判断文件父目录是否存在
        if (!dest.getParentFile().exists()) {
            dest.getParentFile().mkdir();
        }

        try {
            // 保存文件
            file.transferTo(dest);
            return true;
        } catch (IllegalStateException e) {
            e.printStackTrace();
            return false;
        } catch (IOException e) {
            e.printStackTrace();
            return false;
        }

    }


}
