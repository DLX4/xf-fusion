<template>
  <div id="app">
      <div class="box">
          <a-steps :current="current">
              <a-step class="steps" key="可见光图像" title="可见光图像" />
              <a-step class="steps" key="红外图像" title="红外图像" />
              <a-step class="steps" key="算法参数" title="算法参数" />
              <a-step class="steps" key="融合图像" title="融合图像" />
          </a-steps>
          <div class="steps-content">{{steps[current].content}}</div>
          <div class="steps-action">
              <a-button v-if="current<2" type="primary" @click="next">下一步</a-button>
              <a-button v-if="current == 2" type="primary" @click="next">开始融合</a-button>
              <a-button v-if="current == 3" type="primary" @click="$message.success('Processing complete!')">完成</a-button>
              <a-button v-if="current>0" style="margin-left: 8px" @click="prev">上一步</a-button>
          </div>
      </div>

  </div>
</template>

<script>

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
    };
  },
  methods: {
    next() {
      this.current++;
    },
    prev() {
      this.current--;
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
  margin-top: 60px;
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
  margin-top: 16px;
  border: 1px dashed #e9e9e9;
  border-radius: 6px;
  background-color: #fafafa;
  min-height: 200px;
  text-align: center;
  padding-top: 80px;
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
</style>
