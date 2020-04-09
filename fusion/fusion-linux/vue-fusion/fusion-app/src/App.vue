<template>
  <div id="app">
      <div class="box">
          <a-steps :current="current">
              <a-step class="steps" key="可见光图像" title="可见光图像" />
              <a-step class="steps" key="红外图像" title="红外图像" />
              <a-step class="steps" key="算法参数" title="算法参数" />
              <a-step class="steps" key="融合图像" title="融合图像" />
          </a-steps>
          <!--<div class="steps-content" v-if="current > 2">{{steps[current].content}}</div>-->
          <div class="steps-content" v-if="current == 0">
              <a-upload
                      name="image"
                      listType="picture-card"
                      class="image-uploader"
                      :showUploadList="false"
                      action="http://localhost:8088/fileUpload"
                      :beforeUpload="beforeUpload"
                      @change="handleChangeA"
              >
                  <img class="image-uploaded" v-if="fusion.imageUrlA" :src="fusion.imageUrlA" alt="image" />
                  <div v-else>
                      <a-icon :type="loading ? 'loading' : 'plus'" />
                      <div class="ant-upload-text">Upload</div>
                  </div>
              </a-upload>
          </div>

          <div class="steps-content" v-if="current == 1">
              <a-upload
                      name="image"
                      listType="picture-card"
                      class="image-uploader"
                      :showUploadList="false"
                      action="http://localhost:8088/fileUpload"
                      :beforeUpload="beforeUpload"
                      @change="handleChangeB"
              >
                  <img class="image-uploaded" v-if="fusion.imageUrlB" :src="fusion.imageUrlB" alt="image" />
                  <div v-else>
                      <a-icon :type="loading ? 'loading' : 'plus'" />
                      <div class="ant-upload-text">Upload</div>
                  </div>
              </a-upload>
          </div>

          <div class="steps-content" v-if="current == 2">

              <div class="params">
                  <div class="params-row">
                      <div class="params-label ">
                          <label>红外融合策略（顶层）：</label>
                      </div>

                      <a-radio-group class="params-value" defaultValue="a" buttonStyle="solid">
                          <a-radio-button value="a">区域能量</a-radio-button>
                          <a-radio-button value="b">平均梯度</a-radio-button>
                          <a-radio-button value="c">最大亮度</a-radio-button>
                      </a-radio-group>
                  </div>

                  <div class="params-row">
                      <div class="params-label ">
                          <label>可视光融合策略（顶层）：</label>
                      </div>

                      <a-radio-group class="params-value" defaultValue="a" buttonStyle="solid">
                          <a-radio-button value="a">显著性</a-radio-button>
                          <a-radio-button value="b">区域能量</a-radio-button>
                      </a-radio-group>
                  </div>

                  <div class="params-row">
                      <div class="params-label ">
                          <label>红外融合策略（非顶层）：</label>
                      </div>

                      <a-radio-group class="params-value" defaultValue="a" buttonStyle="solid">
                          <a-radio-button value="a">区域能量</a-radio-button>
                          <a-radio-button value="b">平均梯度</a-radio-button>
                          <a-radio-button value="c">最大亮度</a-radio-button>
                      </a-radio-group>
                  </div>

                  <div class="params-row">
                      <div class="params-label ">
                          <label>可视光融合策略（非顶层）：</label>
                      </div>

                      <a-radio-group class="params-value" defaultValue="a" buttonStyle="solid">
                          <a-radio-button value="a">显著性</a-radio-button>
                          <a-radio-button value="b">区域能量</a-radio-button>
                      </a-radio-group>
                  </div>

                  <div class="params-row">
                      <div class="params-label ">
                          <label>LLF 层数：</label>
                      </div>

                      <div class="params-value">
                          <a-input-number  v-model="fusion.level" :min="3" :max="5" />
                      </div>

                  </div>

                  <div class="params-row">
                      <div class="params-label ">
                          <label>LLF-alpha：</label>
                      </div>

                      <div class="params-value">
                          <a-input-number  v-model="fusion.alpha" :min="-10" :max="10" :step="0.1" />
                      </div>

                  </div>

                  <div class="params-row">
                      <div class="params-label ">
                          <label>LLF-beta：</label>
                      </div>

                      <div class="params-value">
                          <a-input-number  v-model="fusion.beta" :min="-10" :max="10" :step="0.1" />
                      </div>

                  </div>

                  <div class="params-row">
                      <div class="params-label ">
                          <label>LLF-sigma：</label>
                      </div>

                      <div class="params-value">
                          <a-input-number  v-model="fusion.sigma" :min="-10" :max="10" :step="0.1" />
                      </div>
                  </div>

                  <div class="params-row">
                      <div class="params-label ">
                          <label>显著性偏差：</label>
                      </div>

                      <div class="params-value">
                          <a-input-number  v-model="fusion.dalta" :min="-10" :max="10" :step="0.1" />
                      </div>
                  </div>
              </div>

          </div>

          <div class="steps-content" v-if="current == 3">
              <div class="card-wrapper">
                  <div class="card">
                      <a-card hoverable style="width: 250px">
                          <img
                                  alt="example"
                                  src="https://os.alipayobjects.com/rmsportal/QBnOOoLaAfKPirc.png"
                                  slot="cover"
                          />
                          <a-card-meta >
                              <template slot="description"
                              >输入可见光图像</template
                              >
                          </a-card-meta>
                      </a-card>
                  </div>

                  <div class="card">
                      <a-card hoverable style="width: 250px">
                          <img
                                  alt="example"
                                  src="https://os.alipayobjects.com/rmsportal/QBnOOoLaAfKPirc.png"
                                  slot="cover"
                          />
                          <a-card-meta >
                              <template slot="description"
                              >输入红外图像</template
                              >
                          </a-card-meta>
                      </a-card>
                  </div>


                  <div class="card">
                      <a-card hoverable style="width: 250px">
                          <img
                                  alt="example"
                                  src="https://os.alipayobjects.com/rmsportal/QBnOOoLaAfKPirc.png"
                                  slot="cover"
                          />
                          <a-card-meta >
                              <template slot="description"
                              >融合图像</template
                              >
                          </a-card-meta>
                      </a-card>
                  </div>
              </div>


          </div>


          <div class="steps-action">
              <a-button v-if="current<2" type="primary" @click="next">下一步</a-button>
              <a-button v-if="current == 2" type="primary" :loading="fusionLoading" @click="doFusion">开始融合</a-button>
              <a-button v-if="current == 3" type="primary" @click="$message.success('Processing complete!')">完成</a-button>
              <a-button v-if="current>0" style="margin-left: 8px" @click="prev">上一步</a-button>
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
import axios from 'axios';

export default {
  name: 'App',
  components: {
  },
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
      loading: false,
      fusionLoading: false,
      fusion : {
        imageUrlA: '',
        imageUrlB: '',
        alpha: 1,
        beta: 1,
        sigma: 1,
        dalta: 1,
        level: 3,
      }
    };
  },
  methods: {
    // Pushes posts to the server when called.
    doFusion() {
      this.fusionLoading = true;
      axios.post(`http://localhost:8088/fusion`, this.fusion)
        .then(response => {
          this.fusionLoading = false;
          this.current++;
          console.log(response)
        })
        .catch(e => {
          console.log(e)
        })

      // async / await version (postPost() becomes async postPost())
      //
      // try {
      //   await axios.post(`http://jsonplaceholder.typicode.com/posts`, {
      //     body: this.postBody
      //   })
      // } catch (e) {
      //   this.errors.push(e)
      // }
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
        this.loading = true;
        return;
      }
      if (info.file.status === 'done') {
        this.fusion.imageUrlA = "http://localhost:8088/show?fileName=" + info.file.response.fileName;
        this.loading = false;
      }
    },
    handleChangeB(info) {

      if (info.file.status === 'uploading') {
        this.loading = true;
        return;
      }
      if (info.file.status === 'done') {
        this.fusion.imageUrlB = "http://localhost:8088/show?fileName=" + info.file.response.fileName;
        this.loading = false;
      }
    },
    beforeUpload(file) {
      const isJpgOrPng = file.type === 'image/jpeg' || file.type === 'image/png';
      if (!isJpgOrPng) {
        this.$message.error('You can only upload JPG file!');
      }
      const isLt2M = file.size / 1024 / 1024 < 2;
      if (!isLt2M) {
        this.$message.error('Image must smaller than 2MB!');
      }
      return isJpgOrPng && isLt2M;
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
    color: rgba(0,0,0,.65);
}

.steps-content {
    margin-top: 40px;
    border: 1px dashed #e9e9e9;
    border-radius: 6px;
    background-color: #fafafa;
    min-height: 580px;
    text-align: center;
}

.steps-action {
  margin-top: 24px;
}

.steps {
    position: relative;
    display: flex;
    flex: 1;
    overflow: hidden;
    vertical-align: top;
}
.steps:last-child{flex:none;}

.params {
    width: 600px;
    position: absolute;
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
    width: 350px;
    text-align: left;
}

.params-row {
    display: flex;
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
    max-height: 500px;
    position: absolute;
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
