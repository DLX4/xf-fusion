import Vue from 'vue'
import {
  Button,
  Steps,
  message,
  Upload,
  Icon,
  Radio,
  Input,
  InputNumber,
  Card,
  Row,
  Col
} from "ant-design-vue";
import 'ant-design-vue/dist/antd.css';
import App from './App.vue'

Vue.use(Button);
Vue.use(Steps);
Vue.use(message);
Vue.use(Upload);
Vue.use(Icon);
Vue.use(Radio);
Vue.use(Input);
Vue.use(InputNumber);
Vue.use(Card);
Vue.use(Row);
Vue.use(Col);

Vue.prototype.$message = message;

Vue.config.productionTip = false

new Vue({
  render: h => h(App),
}).$mount('#app')
