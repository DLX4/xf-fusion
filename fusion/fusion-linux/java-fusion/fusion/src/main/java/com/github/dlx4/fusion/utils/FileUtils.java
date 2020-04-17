package com.github.dlx4.fusion.utils;

import lombok.extern.slf4j.Slf4j;
import org.springframework.web.multipart.MultipartFile;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;

/**
 * 文件相关的工具类
 * @author dinglingxiang
 */
@Slf4j
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
            log.error("error: {}", e.getMessage(), e);
            return false;
        } catch (IOException e) {
            e.printStackTrace();
            log.error("error: {}", e.getMessage(), e);
            return false;
        }

    }

    /**
     * 获取文件后缀
     *
     * @param fileName
     * @return
     */
    public static String getSuffix(String fileName) {
        return fileName.substring(fileName.lastIndexOf("."));
    }

    /**
     * 生成新的文件名
     *
     * @param fileOriginName 源文件名
     * @return
     */
    public static String generateRandomFileName(String fileOriginName) {
        return UUIDUtils.getUUID() + FileUtils.getSuffix(fileOriginName);
    }

    /**
     * 将字符串内容写入到某个文件
     * @param       path
     * @param       content
     * @return void
     */
    public static void writeFile(String path, String content) {
        try {

            File file = new File(path);

            if (!file.exists()) {
                file.createNewFile();
            }

            FileWriter fileWriter = new FileWriter(file, true);

            fileWriter.write(content);
            fileWriter.flush();
            fileWriter.close();

        } catch (IOException e) {
            e.printStackTrace();
            log.error("error: {}", e.getMessage(), e);
        }
    }
}
