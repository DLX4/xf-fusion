<template>
    <div id="app">
        <div class="box">
            <div>
                <a-row>
                    <a-col :span="8">
                        <div class="content">
                            <div class="half-content">
                                <a-upload
                                        name="image"
                                        listType="picture-card"
                                        class="image-uploader"
                                        :showUploadList="false"
                                        action="http://localhost:8088/fileUpload"
                                        :beforeUpload="beforeUpload"
                                        @change="handleChangeA"
                                >
                                    <img class="image-uploaded" v-if="fusion.imageUrlA" :src="fusion.imageUrlA"
                                         alt="image"/>
                                    <div v-else>
                                        <a-icon :type="loadingA ? 'loading' : 'plus'"/>
                                        <div class="ant-upload-text">Upload</div>
                                    </div>
                                </a-upload>

                            </div>
                            <div class="half-content">
                                <a-upload
                                        name="image"
                                        listType="picture-card"
                                        class="image-uploader"
                                        :showUploadList="false"
                                        action="http://localhost:8088/fileUpload"
                                        :beforeUpload="beforeUpload"
                                        @change="handleChangeB"
                                >
                                    <img class="image-uploaded" v-if="fusion.imageUrlB" :src="fusion.imageUrlB"
                                         alt="image"/>
                                    <div v-else>
                                        <a-icon :type="loadingB ? 'loading' : 'plus'"/>
                                        <div class="ant-upload-text">Upload</div>
                                    </div>
                                </a-upload>

                            </div>
                        </div>

                    </a-col>
                    <a-col :span="8">
                        <div class="content">

                            <a-divider>融合模式</a-divider>

                            <div class="params-mode">
                                <div class="middle">
                                    <div class="params-row">
                                        <div class="params-label ">
                                            <label>融合模式：</label>
                                        </div>

                                        <a-radio-group class="params-value" defaultValue="a" buttonStyle="solid"
                                                       v-model="fusion.mode">
                                            <a-radio-button value="a">高性能</a-radio-button>
                                            <a-radio-button value="b">高质量</a-radio-button>
                                            <a-radio-button value="c">普通</a-radio-button>
                                        </a-radio-group>
                                    </div>
                                </div>
                            </div>

                            <a-divider>融合参数</a-divider>

                            <div class="params">

                                <div class="middle">

                                    <div v-show="fusion.mode == 'b'" class="params-row">
                                        <div class="params-label ">
                                            <label>LLF 采样数：</label>
                                        </div>

                                        <div class="params-value">
                                            <a-input-number v-model="fusion.sampling" :min="5" :max="100"/>
                                        </div>

                                    </div>

                                    <div v-show="fusion.mode == 'b'" class="params-row">
                                        <div class="params-label ">
                                            <label>LLF 层数：</label>
                                        </div>

                                        <div class="params-value">
                                            <a-input-number v-model="fusion.level" :min="3" :max="5"/>
                                        </div>

                                    </div>

                                    <div v-show="fusion.mode == 'b'" class="params-row">
                                        <div class="params-label ">
                                            <label>LLF-alpha：</label>
                                        </div>

                                        <div class="params-value">
                                            <a-input-number v-model="fusion.alpha" :min="-10" :max="10" :step="0.1"/>
                                        </div>

                                    </div>

                                    <div v-show="fusion.mode == 'b'" class="params-row">
                                        <div class="params-label ">
                                            <label>LLF-beta：</label>
                                        </div>

                                        <div class="params-value">
                                            <a-input-number v-model="fusion.beta" :min="-10" :max="10" :step="0.1"/>
                                        </div>

                                    </div>

                                    <div v-show="fusion.mode == 'b'" class="params-row">
                                        <div class="params-label ">
                                            <label>LLF-sigma：</label>
                                        </div>

                                        <div class="params-value">
                                            <a-input-number v-model="fusion.sigma" :min="-10" :max="10" :step="0.1"/>
                                        </div>
                                    </div>

                                    <div v-show="fusion.mode == 'b'" class="params-row">
                                        <div class="params-label ">
                                            <label>显著性偏差：</label>
                                        </div>

                                        <div class="params-value">
                                            <a-input-number v-model="fusion.dalta" :min="-10" :max="10" :step="0.1"/>
                                        </div>
                                    </div>
                                    <div v-show="fusion.mode == 'c'" class="params-row">
                                        <div class="params-label ">
                                            <label>金字塔层数：</label>
                                        </div>

                                        <div class="params-value">
                                            <a-input-number v-model="fusion.levelxx" :min="3" :max="5"/>
                                        </div>
                                    </div>

                                </div>

                            </div>


                            <a-divider>实时状态</a-divider>
                            <div class="status">
                                <div class="middle">

                                    <div class="params-row">
                                        <div class="params-label ">
                                            <label>FPS：</label>
                                        </div>
                                        <div class="params-value ">
                                            <label>{{status.fps}}帧/秒</label>
                                        </div>
                                    </div>

                                    <!--<div class="params-row">-->
                                        <!--<div class="params-label ">-->
                                            <!--<label>平均时延：</label>-->
                                        <!--</div>-->
                                        <!--<div class="params-value ">-->
                                            <!--<label>{{status.delay}}毫秒</label>-->
                                        <!--</div>-->
                                    <!--</div>-->
                                </div>
                            </div>

                            <a-divider>控制按钮</a-divider>

                            <div class="control-button">
                                <div class="middle">
                                    <a-row>
                                        <a-col :span="12">
                                            <a-button type="primary" :loading="fusionLoading" @click="doFusion">开始采集
                                            </a-button>
                                        </a-col>

                                        <a-col :span="12">
                                            <a-button @click="$message.success('Processing complete!')">停止</a-button>
                                        </a-col>
                                    </a-row>
                                </div>
                            </div>
                        </div>
                    </a-col>
                    <a-col :span="8">
                        <div class="content">
                            <div class="middle">

                                <img
                                        alt="example"
                                        v-if="fusion.imageUrlS" :src="fusion.imageUrlS"
                                        slot="cover"
                                />

                            </div>

                        </div>
                    </a-col>
                </a-row>
            </div>


        </div>

    </div>
</template>

<script>
  // function getBase64(img, callback) {
  //   const reader = new FileReader();
  //   reader.addEventListener('load', () => callback(reader.result));
  //   reader.readAsDataURL(img);
  // }
  import ACol from "ant-design-vue/es/grid/Col";
  import SockJS from "sockjs-client";
  import Stomp from "stompjs";

  export default {
    name: 'App',
    components: {ACol},
    data() {
      return {
        current: 0,
        steps: [
          {
            title: '可见光图像',
            content: 'First-content',
          },
          {
            title: '红外图像',
            content: 'Second-content',
          },
          {
            title: '算法参数',
            content: 'Third-content',
          },
          {
            title: '融合图像',
            content: 'Last-content',
          },
        ],
        loadingA: false,
        loadingB: false,
        fusionLoading: false,
        fusion: {
          imageUrlA: '',
          imageUrlB: '',
          imageUrlS: '',
          imageA: '',
          imageB: '',
          alpha: 1,
          beta: 1,
          sigma: 1,
          dalta: 1,
          level: 3,
          sampling: 10,
          mode: 'a'
        },
        status: {
          fps:1,
          delay:889
        },
        socket: null,
        stompClient: null
      };
    },
    methods: {
      // Pushes posts to the server when called.
      doFusion() {
        var that = this;
        that.fusionLoading = true;
        that.socket = new SockJS("/ws/zhcx-export-websocket");
        // that.socket = new SockJS("ws://localhost:8088/zhcx-export-websocket");
        that.stompClient = Stomp.over(that.socket);

        that.stompClient.connect({}, function() {
          console.log("连接成功");
          that.stompClient.subscribe("/topic/zhcx", function(res) {
            let response = JSON.parse(res.body);
            console.log(response);
            that.fusion.imageUrlA = "http://localhost:8088/show?fileName=" + response.imageA;
            that.fusion.imageUrlB = "http://localhost:8088/show?fileName=" + response.imageB;
            that.fusion.imageUrlS = "http://localhost:8088/show?fileName=" + response.imageFusion;

          });

          // var sessionId = /\/([^\/]+)\/websocket/.exec(
          //   that.socket._transport.url
          // )[1];
          // that.stompClient.subscribe(
          //   "/user/" + sessionId + "/topic/zhcx",
          //   function(res) {
          //     console.log(res)
          //   }
          // );
        });

      },
      next() {
        this.current++;
      },
      prev() {
        this.current--;
      },
      handleChangeA(info) {
        console.log(info)
        if (info.file.status === 'uploading') {
          this.loadingA = true;
          return;
        }
        if (info.file.status === 'done') {
          this.fusion.imageUrlA = "http://localhost:8088/show?fileName=" + info.file.response.fileName;
          this.fusion.imageA = info.file.response.fileName;
          this.loadingA = false;
        }
      },
      handleChangeB(info) {

        if (info.file.status === 'uploading') {
          this.loadingB = true;
          return;
        }
        if (info.file.status === 'done') {
          this.fusion.imageUrlB = "http://localhost:8088/show?fileName=" + info.file.response.fileName;
          this.fusion.imageB = info.file.response.fileName;
          this.loadingB = false;
        }
      },
      beforeUpload(file) {
        // const isJpgOrPng = file.type === 'image/jpeg' || file.type === 'image/png';
        // if (!isJpgOrPng) {
        //   this.$message.error('You can only upload JPG file!');
        // }
        const isLt2M = file.size / 1024 / 1024 < 2;
        if (!isLt2M) {
          this.$message.error('Image must smaller than 2MB!');
        }
        // return isJpgOrPng && isLt2M;
        return isLt2M;
      },
    },
  }
</script>

<style scoped>
    #app {
        font-family: Avenir, Helvetica, Arial, sans-serif;
        -webkit-font-smoothing: antialiased;
        -moz-osx-font-smoothing: grayscale;
        text-align: center;
        color: #2c3e50;
        margin-top: 10px;
    }

    .box {
        border-radius: 2px;
        width: 100%;
        position: relative;
        margin: 0 0 16px;
        transition: all .2s;
        border: 1px solid #ebedf0;
        padding: 42px 24px 50px;
        color: rgba(0, 0, 0, .65);
    }

    .content {
        position: relative;
        margin-top: 40px;
        border: 1px dashed #e9e9e9;
        border-radius: 6px;
        background-color: #fafafa;
        width: 100%;
        height: 800px;
        text-align: center;
    }

    .half-content {
        position: relative;
        height: 400px;
        width: 100%;
    }

    .control-button {
        position: relative;
        height: 10%;
        width: 550px;
    }

    .status {
        position: relative;
        height: 5%;
        width: 100%;
    }

    .params {
        position: relative;
        height: 45%;
        width: 100%;
        /*border: 1px solid #ebedf0;*/
    }

    .params-mode {
        position: relative;
        height: 10%;
        width: 100%;
        /*border: 1px solid #ebedf0;*/
    }

    /*    .steps-action {
            margin-top: 24px;
        }

        .steps {
            position: relative;
            display: flex;
            flex: 1;
            overflow: hidden;
            vertical-align: top;
        }

        .steps:last-child {
            flex: none;
        }*/

    .middle {
        position: relative;
        top: 50%;
        left: 50%;
        transform: translate(-50%, -50%);
    }

    .params-label {
        width: 200px;
        float: left;
        text-align: right;
        color: rgb(51, 51, 51);
        margin-bottom: 0;
    }

    .params-value {
        width: 250px;
        text-align: left;
    }

    .params-row {
        display: flex;
        width: 550px;
        height: 40px;
        flex-wrap: wrap;
        line-height: 40px;
    }

    .image-uploader {
        position: absolute;
        top: 50%;
        left: 50%;
        transform: translate(-50%, -50%);
        width: 128px;
        height: 128px;
    }

    .image-uploader > .ant-upload {
        width: 128px;
        height: 128px;
    }

    .image-uploaded {
        position: absolute;
        max-height: 500px;
        top: 50%;
        left: 50%;
        transform: translate(-50%, -50%);
    }

    .ant-upload-select-picture-card i {
        font-size: 32px;
        color: #999;
    }

    .ant-upload-select-picture-card .ant-upload-text {
        margin-top: 8px;
        color: #666;
    }

    .card-wrapper {
        flex-wrap: wrap;
        display: flex;
        position: absolute;
        top: 50%;
        left: 50%;
        transform: translate(-50%, -50%);
        width: 90%;
        padding: 42px 24px 50px;
    }

    .card {
        position: relative;
        flex: 1;
        overflow: hidden;
    }
</style>
