package com.github.dlx4.fusion;

import com.github.dlx4.fusion.controller.FusionController;
import com.github.dlx4.fusion.model.FusionParams;
import com.github.dlx4.fusion.model.FusionResponse;
import com.github.dlx4.fusion.utils.SpringUtils;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.context.ApplicationContext;
import org.springframework.messaging.converter.MappingJackson2MessageConverter;
import org.springframework.messaging.simp.stomp.*;
import org.springframework.web.socket.WebSocketHttpHeaders;
import org.springframework.web.socket.client.standard.StandardWebSocketClient;
import org.springframework.web.socket.messaging.WebSocketStompClient;
import org.springframework.web.socket.sockjs.client.SockJsClient;
import org.springframework.web.socket.sockjs.client.Transport;
import org.springframework.web.socket.sockjs.client.WebSocketTransport;

import java.lang.reflect.Type;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicReference;

@SpringBootApplication
public class FusionApplication {


    public static void main(String[] args) {
        SpringApplication.run(FusionApplication.class, args);

        ApplicationContext context = SpringUtils.getApplicationContext();

        FusionController fusionController = context.getBean(FusionController.class);

        // 添加测试数据集
        List<FusionParams> list = new LinkedList<>();
        list.add(new FusionParams("1800v.bmp", "1800i.bmp"));
        list.add(new FusionParams("1801v.bmp", "1801i.bmp"));
        list.add(new FusionParams("1802v.bmp", "1802i.bmp"));
        list.add(new FusionParams("1803v.bmp", "1803i.bmp"));
        list.add(new FusionParams("1804v.bmp", "1804i.bmp"));
        list.add(new FusionParams("1805v.bmp", "1805i.bmp"));
        list.add(new FusionParams("1806v.bmp", "1806i.bmp"));
        list.add(new FusionParams("1807v.bmp", "1807i.bmp"));
        list.add(new FusionParams("1808v.bmp", "1808i.bmp"));
        list.add(new FusionParams("1809v.bmp", "1809i.bmp"));
        list.add(new FusionParams("1810v.bmp", "1810i.bmp"));
        list.add(new FusionParams("1811v.bmp", "1811i.bmp"));
        list.add(new FusionParams("1812v.bmp", "1812i.bmp"));
        list.add(new FusionParams("1813v.bmp", "1813i.bmp"));
        list.add(new FusionParams("1814v.bmp", "1814i.bmp"));
        list.add(new FusionParams("1815v.bmp", "1815i.bmp"));
        list.add(new FusionParams("1816v.bmp", "1816i.bmp"));
        list.add(new FusionParams("1817v.bmp", "1817i.bmp"));
        list.add(new FusionParams("1818v.bmp", "1818i.bmp"));
        list.add(new FusionParams("1819v.bmp", "1819i.bmp"));
        list.add(new FusionParams("1820v.bmp", "1820i.bmp"));
        list.add(new FusionParams("1821v.bmp", "1821i.bmp"));
        list.add(new FusionParams("1822v.bmp", "1822i.bmp"));
        list.add(new FusionParams("1823v.bmp", "1823i.bmp"));
        list.add(new FusionParams("1824v.bmp", "1824i.bmp"));
        list.add(new FusionParams("1825v.bmp", "1825i.bmp"));
        list.add(new FusionParams("1826v.bmp", "1826i.bmp"));
        list.add(new FusionParams("1827v.bmp", "1827i.bmp"));
        list.add(new FusionParams("1828v.bmp", "1828i.bmp"));
        list.add(new FusionParams("1829v.bmp", "1829i.bmp"));
        list.add(new FusionParams("1830v.bmp", "1830i.bmp"));
        list.add(new FusionParams("1831v.bmp", "1831i.bmp"));


        SockJsClient sockJsClient;

        WebSocketStompClient stompClient;

        final WebSocketHttpHeaders headers = new WebSocketHttpHeaders();

        List<Transport> transports = new ArrayList<>();
        transports.add(new WebSocketTransport(new StandardWebSocketClient()));
        sockJsClient = new SockJsClient(transports);

        stompClient = new WebSocketStompClient(sockJsClient);
        stompClient.setMessageConverter(new MappingJackson2MessageConverter());


        // 循环融合数据集
        while (true) {
            list.forEach(fusionParams -> {
                try {

                    final CountDownLatch latch = new CountDownLatch(1);
                    final AtomicReference<Throwable> failure = new AtomicReference<>();

                    StompSessionHandler handler = new TestSessionHandler(failure) {

                        @Override
                        public void afterConnected(final StompSession session, StompHeaders connectedHeaders) {
                            session.subscribe("/topic/zhcx", new StompFrameHandler() {
                                @Override
                                public Type getPayloadType(StompHeaders headers) {
                                    return FusionResponse.class;
                                }

                                @Override
                                public void handleFrame(StompHeaders headers, Object payload) {
                                    FusionResponse response = (FusionResponse) payload;
                                    try {
                                        System.out.println(response);
                                    } catch (Throwable t) {
                                        failure.set(t);
                                    } finally {
                                        session.disconnect();
                                        latch.countDown();
                                    }
                                }
                            });
                            try {
                                System.out.println("[采集下一帧]" + fusionParams);
                                session.send("/zhcx/export", fusionParams);
                            } catch (Throwable t) {
                                failure.set(t);
                                latch.countDown();
                            }
                        }
                    };

                    stompClient.connect("ws://localhost:8088/zhcx-export-websocket", headers, handler, 8088);

                    if (latch.await(5, TimeUnit.SECONDS)) {
                        if (failure.get() != null) {
                            throw new AssertionError("", failure.get());
                        }
                    } else {
                        System.out.println("Greeting not received");
                    }

                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            });
        }
    }

    private static class TestSessionHandler extends StompSessionHandlerAdapter {

        private final AtomicReference<Throwable> failure;

        public TestSessionHandler(AtomicReference<Throwable> failure) {
            this.failure = failure;
        }

        @Override
        public void handleFrame(StompHeaders headers, Object payload) {
            this.failure.set(new Exception(headers.toString()));
        }

        @Override
        public void handleException(StompSession s, StompCommand c, StompHeaders h, byte[] p, Throwable ex) {
            this.failure.set(ex);
        }

        @Override
        public void handleTransportError(StompSession session, Throwable ex) {
            this.failure.set(ex);
        }
    }

}
