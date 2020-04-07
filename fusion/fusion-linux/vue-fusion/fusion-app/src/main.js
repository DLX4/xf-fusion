import Vue from 'vue'
import {
  Button,
  Steps,
  message,
  Upload,
  Icon,
//  Layout,
//  Menu,
//
//  Select,
//  TimePicker,
//  DatePicker,
//  LocaleProvider,
//  Table,
//  input,
//  Switch,
//  Radio,

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

Vue.prototype.$message = message;

Vue.config.productionTip = false

new Vue({
  render: h => h(App),
}).$mount('#app')
