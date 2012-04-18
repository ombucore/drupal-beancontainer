<?php

/**
 * @file
 * Bean plugin object for bean container.
 */

class BeanContainer extends BeanPlugin {
  /**
   * @see: parent::values().
   */
  public function values() {
    $values = parent::values();

    $values['children'] = array();

    return $values;
  }

  /**
   * Return the block content.
   *
   * @param $bean
   *   The bean object being viewed.
   * @param $content
   *   The default content array created by Entity API.  This will include any
   *   fields attached to the entity.
   * @param $view_mode
   *   The view mode passed into $entity->view().
   * @return
   *   Return a renderable content array.
   */
  public function view($bean, $content, $view_mode = 'default', $langcode = NULL) {
    // Build lists for navigation and tabs.
    $tabs = array();
    $nav  = array();

    if (!empty($bean->children)) {
      $i = 1;
      foreach ($bean->children as $child) {
        $child_bean = bean_load($child);

        $nav[] = array(
          'data' => $child_bean->title,
          'class' => array('nav-item-' . $i),
        );

        $bean_content = $child_bean->view($view_mode);
        $tabs[] = array(
          'data' => drupal_render($bean_content),
          'class' => array('item-' . $i),
        );

        $i++;
      }
    }

    // @todo: might want to spin this off into a theme function.
    $content['bean'][$bean->bid]['children'] = array(
      'navigation' => array(
        '#theme' => 'item_list',
        '#items' => $nav,
        '#attributes' => array(
          'class' => array('bean-container-nav'),
        ),
      ),
      'tabs' => array(
        '#theme' => 'item_list',
        '#items' => $tabs,
        '#attributes' => array(
          'class' => array('bean-container-tabs'),
        ),
      ),
    );

    return $content;
  }
}
