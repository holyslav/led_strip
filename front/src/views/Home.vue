<template>
  <v-container>
    <v-layout text-xs-center wrap>
      <v-flex xs12 sm6 offset-sm3>
        <color-picker v-model="color"></color-picker>
        <div class="selected-color-info">
          <p>Current color:</p>
          <svg height="32" width="32">
            <circle cx="16" cy="16" r="15" :fill="color" />
          </svg>
        </div>
      </v-flex>
    </v-layout>
  </v-container>
</template>

<script>
import ColorPicker from 'vue-color-picker-wheel'

export default {
  name: 'Home',
  components: {
    ColorPicker
  },
  data () {
    return {
      color: this.getColor()
    }
  },
  methods: {
    componentToHex: function (c) {
      var hex = c.toString(16)
      return hex.length === 1 ? '0' + hex : hex
    },
    convertRgbToHex: function (color) {
      return '#' + this.componentToHex(color.red) + this.componentToHex(color.green) + this.componentToHex(color.blue)
    },
    convertHex2Rgb: function (color) {
      color = color.replace('#', '')
      let r = parseInt(color.substring(0, 2), 16)
      let g = parseInt(color.substring(2, 4), 16)
      let b = parseInt(color.substring(4, 6), 16)
      return { red: r, green: g, blue: b }
    },
    getColor: function () {
      this.$ajax
        .get('/api/v1/color')
        .then(response => {
          console.log(response)
          this.color = this.convertRgbToHex(response.data)
        })
        .catch(error => {
          console.log(error)
        })
      return '#538cdb'
    },
    setColor: function (color) {
      this.$ajax
        .post('/api/v1/color', color)
        .then(response => {
          console.log(response)
        })
        .catch(error => {
          console.log(error)
        })
    }
  },
  watch: {
    color: function (val) {
      console.log('color', val, this.convertHex2Rgb(val))
      this.setColor(this.convertHex2Rgb(val))
    }
  }
}
</script>

<style>
@import url(https://fonts.googleapis.com/css?family=Barlow);

#app {
  font-family: "Barlow", Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  color: #2c3e50;
  text-align: center;
  margin-top: 20px;
}

#color-wheel {
  margin-left: auto;
  margin-right: auto;
}

.selected-color-info {
  display: flex;
  justify-content: center;
  align-items: center;
  margin-top: 10px;
}

.selected-color-info p {
  margin: 0 5px 0 0;
}
</style>
