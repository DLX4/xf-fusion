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
//  Layout,
//  Menu,
//
//  Select,
//  TimePicker,
//  DatePicker,
//  LocaleProvider,
//  Table,

//  Switch,


//  checkbox,
//  Transfer,
//  table,
//  spin,
//  notification
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

Vue.prototype.$message = message;

Vue.config.productionTip = false

new Vue({
  render: h => h(App),
}).$mount('#app')
