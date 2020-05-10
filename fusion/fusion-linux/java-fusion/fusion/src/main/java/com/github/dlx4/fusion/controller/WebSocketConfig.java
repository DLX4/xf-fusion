package com.github.dlx4.fusion.controller;

import org.springframework.context.annotation.Configuration;
import org.springframework.messaging.simp.config.MessageBrokerRegistry;
import org.springframework.web.socket.config.annotation.EnableWebSocketMessageBroker;
import org.springframework.web.socket.config.annotation.StompEndpointRegistry;
import org.springframework.web.socket.config.annotation.WebSocketMessageBrokerConfigurer;

@Configuration
@EnableWebSocketMessageBroker
public class WebSocketConfig implements WebSocketMessageBrokerConfigurer {

    @Override
    public void configureMessageBroker(MessageBrokerRegistry config) {
        // we set up a simple memory-based message broker to
        // carry the messages back to the client on destinations prefixed with /topic
        config.enableSimpleBroker("/topic", "/user");
        config.setApplicationDestinationPrefixes("/zhcx");
    }

    @Override
    public void registerStompEndpoints(StompEndpointRegistry registry) {
        // registered stomp endpoints at zhcx-export-websocket
        registry.addEndpoint("/zhcx-export-websocket")
                .setAllowedOrigins("*").withSockJS();

    }

}
